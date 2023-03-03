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
#include "json_project_serializer.h"

namespace esc::coreui {
///
auto Project::CreateProject() const {
  auto id_generator = core::IdGenerator{};
  auto families = std::vector<std::unique_ptr<core::IFamily>>{};

  for (const auto& generation : generations_) {
    auto generation_families = generation->CreateFamilies(id_generator);

    families.insert(families.end(),
                    std::move_iterator{generation_families.begin()},
                    std::move_iterator{generation_families.end()});
  }

  return core::Project{core::Settings{
                           .min_flow = -27.F,
                           .low_flow = -22.F,
                           .high_flow = -18.F,
                           .max_flow = 6.F,
                       },
                       std::move(families), core::Diagram{}};
}

///
Project::Project(std::vector<std::unique_ptr<core::IGeneration>> generations,
                 TexturesHandle textures_handle)
    : generations_{std::move(generations)},
      textures_handle_{std::move(textures_handle)},
      project_{CreateProject()},
      diagram_{safe_owner_.MakeSafe(&project_.GetDiagram()),
               safe_owner_.MakeSafe(&project_.GetFamilies()),
               safe_owner_.MakeSafe(&project_.GetIdGenerator()),
               {.is_color_flow =
                    [safe_this = safe_owner_.MakeSafe(this)]() {
                      return safe_this->project_.GetSettings().color_flow;
                    },
                .get_flow_color =
                    [safe_this = safe_owner_.MakeSafe(this)](auto flow) {
                      return core::Settings::GetFlowColor(
                          safe_this->project_.GetSettings(), flow);
                    },
                .post_event =
                    [safe_this = safe_owner_.MakeSafe(this)](auto event) {
                      safe_this->event_loop_.PostEvent(std::move(event));
                    },
                .get_texture = [safe_this = safe_owner_.MakeSafe(this)](
                                   auto file_path) -> const Texture& {
                  return safe_this->textures_handle_.GetTexture(file_path);
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
  family_parsers.reserve(generations_.size());

  std::transform(
      generations_.begin(), generations_.end(),
      std::back_inserter(family_parsers),
      [](const auto& generation) { return generation->CreateFamilyParser(); });

  return family_parsers;
}

///
void Project::OpenFromFile(std::string file_path) {
  event_loop_.PostEvent([safe_this = safe_owner_.MakeSafe(this),
                         file_path = std::move(file_path)]() {
    const auto json = crude_json::value::load(file_path).first;
    safe_this->project_ = json::ProjectSerializer::ParseFromJson(
        json, safe_this->CreateFamilyParsers());
  });
}

///
void Project::SaveToFile(std::string file_path) {
  event_loop_.PostEvent([safe_this = safe_owner_.MakeSafe(this),
                         file_path = std::move(file_path)]() {
    const auto json = json::ProjectSerializer::WriteToJson(safe_this->project_);
    json.save(file_path);
  });
}

///
void Project::Reset() {
  event_loop_.PostEvent([safe_this = safe_owner_.MakeSafe(this)]() {
    safe_this->project_ = safe_this->CreateProject();
  });
}
}  // namespace esc::coreui