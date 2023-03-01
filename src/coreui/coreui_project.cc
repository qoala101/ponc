#include "coreui_project.h"

#include <functional>
#include <utility>
#include <variant>
#include <vector>

#include "app_attenuator_node.h"
#include "app_input_node.h"
#include "core_diagram.h"
#include "coreui_diagram.h"
#include "coreui_event_loop.h"
#include "json_i_family_parser.h"
#include "json_project_serializer.h"

namespace esc::coreui {
namespace {
auto CreateFamilyParsers() {
  auto family_parsers = std::vector<std::unique_ptr<json::IFamilyParser>>{};
  family_parsers.emplace_back(InputNode::CreateFamilyParser());
  // family_parsers.emplace_back(ClientNode::CreateFamilyParser());
  // family_parsers.emplace_back(CouplerNode::CreateFamilyParser());
  // family_parsers.emplace_back(SplitterNode::CreateFamilyParser());
  family_parsers.emplace_back(AttenuatorNode::CreateFamilyParser());
  // family_parsers.emplace_back(core::PlaceholderFamily::CreateParser());
  return family_parsers;
}
}  // namespace

Project::Project(cpp::SafePointer<core::Project> project)
    : project_{std::move(project)},
      diagram_{project_->GetDiagram().CreateSafePointer(),
               event_loop_.CreateSafePointer()} {}

void Project::OnFrame() {
  event_loop_.OnFrame();
  diagram_.OnFrame(*project_);
}

auto Project::GetDiagram() const -> const Diagram & {
  // NOLINTNEXTLINE(*-const-cast)
  return const_cast<Project *>(this)->GetDiagram();
}

auto Project::GetDiagram() -> Diagram & { return diagram_; }

void Project::Event_OpenFromFile(std::string file_path) {
  event_loop_.events_.emplace_back(
      [project = project_, file_path = std::move(file_path)]() {
        const auto json = crude_json::value::load(file_path).first;
        *project =
            json::ProjectSerializer::ParseFromJson(json, CreateFamilyParsers());
      });
}

void Project::Event_SaveToFile(std::string file_path) {
  event_loop_.events_.emplace_back(
      [project = project_, file_path = std::move(file_path)]() {
        const auto json = json::ProjectSerializer::WriteToJson(*project);
        json.save(file_path);
      });
}
}  // namespace esc::coreui