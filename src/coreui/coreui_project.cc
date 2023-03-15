#include "coreui_project.h"

#include <algorithm>
#include <filesystem>
#include <functional>
#include <iterator>
#include <memory>
#include <utility>
#include <variant>
#include <vector>

#include "core_diagram.h"
#include "core_i_family_group.h"
#include "core_id_generator.h"
#include "core_project.h"
#include "core_settings.h"
#include "coreui_diagram.h"
#include "coreui_event_loop.h"
#include "cpp_assert.h"
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
                       std::move(families), std::vector<core::Diagram>(1)};
}

///
auto Project::CreateDiagram() {
  auto& diagrams = project_.GetDiagrams();
  Expects(!diagrams.empty());

  return std::make_unique<Diagram>(safe_owner_.MakeSafe(this),
                                   safe_owner_.MakeSafe(&diagrams.front()));
}

///
Project::Project(std::vector<std::unique_ptr<core::IFamilyGroup>> family_groups,
                 TexturesHandle textures_handle, Callbacks callbacks)
    : family_groups_{[&family_groups]() {
        auto default_family_groups =
            core::IFamilyGroup::CreateDefaultFamilyGroups();

        family_groups.insert(family_groups.end(),
                             std::move_iterator{default_family_groups.begin()},
                             std::move_iterator{default_family_groups.end()});

        return std::move(family_groups);
      }()},
      textures_handle_{std::move(textures_handle)},
      callbacks_{std::move(callbacks)},
      project_{CreateProject()},
      diagram_{CreateDiagram()} {
  callbacks_.name_changed(GetName());
}

///
void Project::OnFrame() {
  event_loop_.ExecuteEvents();
  diagram_->OnFrame();
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
auto Project::GetDiagram() -> Diagram& { return *diagram_; }

///
auto Project::GetTexturesHandle() -> TexturesHandle& {
  return textures_handle_;
}

///
auto Project::GetEventLoop() -> EventLoop& { return event_loop_; }

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
void Project::Reset() {
  event_loop_.PostEvent([safe_this = safe_owner_.MakeSafe(this),
                         new_project = cpp::Share(CreateProject())]() {
    safe_this->project_ = std::move(*new_project);
    safe_this->diagram_ = safe_this->CreateDiagram();
    safe_this->SetFilePath({});
  });
}

///
void Project::OpenFromFile(std::filesystem::path file_path) {
  event_loop_.PostEvent([safe_this = safe_owner_.MakeSafe(this),
                         family_parsers = cpp::Share(CreateFamilyParsers()),
                         file_path = std::move(file_path)]() mutable {
    const auto json = crude_json::value::load(file_path).first;
    safe_this->project_ =
        json::ProjectSerializer::ParseFromJson(json, *family_parsers);
    safe_this->diagram_ = safe_this->CreateDiagram();
    safe_this->SetFilePath(std::move(file_path));
    safe_this->event_loop_.PostEvent([]() { ne::NavigateToContent(); });
  });
}

///
auto Project::CanSave() const -> bool { return !file_path_.empty(); }

///
void Project::Save() {
  Expects(!file_path_.empty());
  SaveToFile(file_path_);
}

///
void Project::SaveToFile(std::filesystem::path file_path) {
  event_loop_.PostEvent([safe_this = safe_owner_.MakeSafe(this),
                         file_path = std::move(file_path)]() mutable {
    const auto json = json::ProjectSerializer::WriteToJson(safe_this->project_);
    json.save(file_path);
    safe_this->SetFilePath(std::move(file_path));
  });
}

///
auto Project::GetName() const -> std::string {
  if (file_path_.empty()) {
    return "Unknown";
  }

  return file_path_.filename();
}

///
void Project::SetFilePath(std::filesystem::path file_path) {
  file_path_ = std::move(file_path);
  callbacks_.name_changed(GetName());
}
}  // namespace esc::coreui