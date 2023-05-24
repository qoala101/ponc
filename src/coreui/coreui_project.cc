/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "coreui_project.h"

#include <crude_json.h>

#include <algorithm>
#include <filesystem>
#include <functional>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "core_diagram.h"
#include "core_i_family.h"
#include "core_i_family_group.h"
#include "core_id_generator.h"
#include "core_project.h"
#include "core_settings.h"
#include "coreui_cloner.h"
#include "coreui_diagram.h"
#include "coreui_event.h"
#include "coreui_event_loop.h"
#include "coreui_log.h"
#include "cpp_assert.h"
#include "cpp_share.h"
#include "json_i_family_parser.h"
#include "json_project_serializer.h"
#include "json_versifier.h"

namespace vh::ponc::coreui {
///
auto Project::CreateProject() const {
  auto id_generator = core::IdGenerator{};
  auto families = std::vector<std::unique_ptr<core::IFamily>>{};

  for (const auto& family_group : family_groups_) {
    auto group_families = family_group->CreateFamilies(id_generator);

    families.insert(families.cend(), std::move_iterator{group_families.begin()},
                    std::move_iterator{group_families.end()});
  }

  auto settings = core::Settings{
      .calculator_settings = {
          .family_settings =
              core::CalculatorFamilySettings::FromFamilies(families)}};
  core::Settings::ResetToDefault(settings);

  auto project = core::Project{std::move(settings), std::move(families), {}};
  project.EmplaceDiagram(
      core::Diagram{core::Project::MakeUniqueDiagramName(project)});

  return project;
}

///
Project::Project(std::vector<std::unique_ptr<core::IFamilyGroup>> family_groups,
                 Callbacks callbacks)
    : family_groups_{[&family_groups]() {
        auto default_family_groups =
            core::IFamilyGroup::CreateDefaultFamilyGroups();

        family_groups.insert(family_groups.cend(),
                             std::move_iterator{default_family_groups.begin()},
                             std::move_iterator{default_family_groups.end()});

        return std::move(family_groups);
      }()},
      callbacks_{std::move(callbacks)},
      project_{CreateProject()},
      calculator_{safe_owner_.MakeSafe(this)} {
  SetDiagramImpl(0);
  callbacks_.name_changed(GetName());
}

///
void Project::OnFrame() {
  event_loop_.ExecuteEvents();
  diagram_->OnFrame();
  calculator_.OnFrame();
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
auto Project::AddDiagram(core::Diagram diagram) -> Event& {
  return event_loop_.PostEvent(
      [safe_this = safe_owner_.MakeSafe(this),
       diagram = cpp::Share(std::move(diagram))]() mutable {
        auto& added_diagram =
            safe_this->project_.EmplaceDiagram(std::move(*diagram));

        safe_this->diagram_ = std::make_unique<Diagram>(
            safe_this, safe_this->safe_owner_.MakeSafe(&added_diagram));
      });
}

///
auto Project::CloneDiagram(const core::Diagram& diagram) -> Event& {
  auto clone = Cloner::Clone(diagram, project_.GetFamilies());
  Cloner::RewireIds(core::Diagram::GetIds(clone), project_.GetIdGenerator());

  auto new_name =
      core::Project::MakeUniqueDiagramName(project_, diagram.GetName(), "copy");
  clone.SetName(std::move(new_name));

  return AddDiagram(std::move(clone));
}

///
auto Project::DeleteDiagram(int index) -> Event& {
  return event_loop_.PostEvent(
      [index, safe_this = safe_owner_.MakeSafe(this)]() mutable {
        safe_this->project_.DeleteDiagram(index);

        const auto num_diagrams =
            static_cast<int>(safe_this->project_.GetDiagrams().size());
        const auto next_index = std::min(index, num_diagrams - 1);

        safe_this->SetDiagramImpl(next_index);
      });
}

///
auto Project::SetDiagram(int index) -> Event& {
  return event_loop_.PostEvent(
      [index, safe_this = safe_owner_.MakeSafe(this)]() {
        safe_this->SetDiagramImpl(index);
      });
}

///
auto Project::GetCalculator() -> Calculator& { return calculator_; }

///
auto Project::GetLog() -> Log& { return log_; }

///
auto Project::GetEventLoop() -> EventLoop& { return event_loop_; }

///
auto Project::CreateFamilyParsers() const {
  auto family_parsers = std::vector<std::unique_ptr<json::IFamilyParser>>{};
  family_parsers.reserve(family_groups_.size());

  std::transform(family_groups_.cbegin(), family_groups_.cend(),
                 std::back_inserter(family_parsers),
                 [](const auto& family_group) {
                   return family_group->CreateFamilyParser();
                 });

  return family_parsers;
}

///
auto Project::Reset() -> Event& {
  return event_loop_.PostEvent([safe_this = safe_owner_.MakeSafe(this),
                                new_project = cpp::Share(CreateProject())]() {
    safe_this->project_ = std::move(*new_project);
    safe_this->SetDiagramImpl(0);
    safe_this->SetFilePath({});

    safe_this->log_.Write(LogLevel::kInfo, "Created new project");
  });
}

///
auto Project::OpenFromFile(std::filesystem::path file_path) -> Event& {
  return event_loop_.PostEvent([safe_this = safe_owner_.MakeSafe(this),
                                family_parsers =
                                    cpp::Share(CreateFamilyParsers()),
                                file_path = std::move(file_path)]() mutable {
    auto json = crude_json::value::load(file_path.string()).first;
    json::Versifier::UpgradeToCurrentVersion(json);

    safe_this->project_ =
        json::ProjectSerializer::ParseFromJson(json, *family_parsers);

    safe_this->SetDiagramImpl(0);
    safe_this->SetFilePath(std::move(file_path));

    safe_this->log_.Write(LogLevel::kInfo, "Opened project from " +
                                               safe_this->file_path_.string());
  });
}

///
auto Project::CanSave() const -> bool { return !file_path_.empty(); }

///
auto Project::Save() -> Event& { return SaveToFile(file_path_); }

///
auto Project::SaveToFile(std::filesystem::path file_path) -> Event& {
  return event_loop_.PostEvent([safe_this = safe_owner_.MakeSafe(this),
                                file_path = std::move(file_path)]() mutable {
    const auto json = json::ProjectSerializer::WriteToJson(safe_this->project_);
    json.save(file_path.string());
    safe_this->SetFilePath(std::move(file_path));

    safe_this->log_.Write(LogLevel::kInfo,
                          "Saved project to " + safe_this->file_path_.string());
  });
}

///
auto Project::GetName() const -> std::string {
  if (file_path_.empty()) {
    return "Unknown";
  }

  return file_path_.filename().string();
}

///
void Project::SetDiagramImpl(int index) {
  auto& diagrams = project_.GetDiagrams();
  Expects(static_cast<int>(diagrams.size()) > index);

  diagram_ = std::make_unique<Diagram>(safe_owner_.MakeSafe(this),
                                       safe_owner_.MakeSafe(&diagrams[index]));
}

///
void Project::SetFilePath(std::filesystem::path file_path) {
  file_path_ = std::move(file_path);
  callbacks_.name_changed(GetName());
}
}  // namespace vh::ponc::coreui