#include "coreui_project.h"

#include <algorithm>
#include <functional>
#include <iterator>
#include <utility>
#include <variant>
#include <vector>

#include "core_diagram.h"
#include "core_id_generator.h"
#include "core_project.h"
#include "core_settings.h"
#include "coreui_diagram.h"
#include "coreui_event_loop.h"
#include "coreui_texture.h"
#include "cpp_share.h"
#include "json_project_serializer.h"

namespace esc::coreui {
///
auto Project::CreateProject() const {
  auto id_generator = core::IdGenerator{};
  auto families = std::vector<std::unique_ptr<core::IFamily>>{};

  for (const auto& family_group : family_groups_) {
    auto group_families = family_group->CreateFamilies(id_generator);

    families.insert(families.end(), std::move_iterator{group_families.begin()},
                    std::move_iterator{group_families.end()});
  }

  return core::Project{core::Settings{
                           .min_flow = -27,
                           .low_flow = -22,
                           .high_flow = -18,
                           .max_flow = 6,
                       },
                       std::move(families), core::Diagram{}};
}

///
Project::Project(std::vector<std::unique_ptr<core::IFamilyGroup>> family_groups,
                 TexturesHandle textures_handle)
    : family_groups_{std::move(family_groups)},
      textures_handle_{std::move(textures_handle)},
      project_{CreateProject()},
      diagram_{
          safe_owner_.MakeSafe(&project_.GetDiagram()),
          safe_owner_.MakeSafe(&project_.GetFamilies()),
          safe_owner_.MakeSafe(&project_.GetIdGenerator()),
          {.is_color_flow =
               [color_flow = safe_owner_.MakeSafe(
                    &project_.GetSettings().color_flow)]() {
                 return *color_flow;
               },
           .get_flow_color =
               [settings =
                    safe_owner_.MakeSafe(&project_.GetSettings())](auto flow) {
                 return core::Settings::GetFlowColor(*settings, flow);
               },
           .get_texture = [textures_handle =
                               safe_owner_.MakeSafe(&textures_handle_)](
                              auto file_path) -> const Texture& {
             return textures_handle->GetTexture(file_path);
           },
           .post_event =
               [event_loop = safe_owner_.MakeSafe(&event_loop_)](auto event) {
                 event_loop->PostEvent(std::move(event));
               }}} {}

///
void Project::OnFrame() {
  event_loop_.ExecuteEvents();
  diagram_.OnFrame();
}

///
auto Project::GetProject() const -> const core::Project& {
  // NOLINTNEXTLINE(*-const-cast)
  return const_cast<Project*>(this)->GetProject();
}

///
auto Project::GetProject() -> core::Project& { return project_; }

///
auto Project::GetDiagram() const -> const Diagram& {
  // NOLINTNEXTLINE(*-const-cast)
  return const_cast<Project*>(this)->GetDiagram();
}

///
auto Project::GetDiagram() -> Diagram& { return diagram_; }

///
auto Project::CreateFamilyParsers() const {
  auto family_parsers = std::vector<std::unique_ptr<json::IFamilyParser>>{};
  family_parsers.reserve(family_groups_.size());

  std::transform(family_groups_.begin(), family_groups_.end(),
                 std::back_inserter(family_parsers),
                 [](const auto& family_group) {
                   return family_group->CreateFamilyParser();
                 });

  return family_parsers;
}

///
void Project::OpenFromFile(std::string file_path) {
  event_loop_.PostEvent([project = safe_owner_.MakeSafe(&project_),
                         family_parsers = cpp::Share(CreateFamilyParsers()),
                         file_path = std::move(file_path)]() {
    const auto json = crude_json::value::load(file_path).first;
    *project = json::ProjectSerializer::ParseFromJson(json, *family_parsers);
  });
}

///
void Project::SaveToFile(std::string file_path) {
  event_loop_.PostEvent([project = safe_owner_.MakeSafe(&project_),
                         file_path = std::move(file_path)]() {
    const auto json = json::ProjectSerializer::WriteToJson(*project);
    json.save(file_path);
  });
}

///
void Project::Reset() {
  event_loop_.PostEvent([project = safe_owner_.MakeSafe(&project_),
                         new_project = cpp::Share(CreateProject())]() mutable {
    *project = std::move(*new_project);
  });
}
}  // namespace esc::coreui