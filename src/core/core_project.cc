#include "core_project.h"

#include <algorithm>
#include <concepts>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <stack>
#include <string>
#include <vector>

#include "core_diagram.h"
#include "core_id_generator.h"
#include "core_id_value.h"
#include "cpp_assert.h"
#include "flow_tree_traversal.h"
#include "imgui_node_editor.h"

namespace vh::ponc::core {
///
auto Project::FindMaxId() const {
  auto max_id = UnspecifiedIdValue{1};

  for (const auto& family : families_) {
    max_id = std::max(family->GetId().Get(), max_id);
  }

  for (const auto& diagram : diagrams_) {
    for (const auto& node : diagram.GetNodes()) {
      max_id = std::max(node->GetId().Get(), max_id);

      for (const auto& pin : INode::GetAllPins(*node)) {
        max_id = std::max(pin.first.Get(), max_id);
      }
    }

    for (const auto& link : diagram.GetLinks()) {
      max_id = std::max(link.id.Get(), max_id);
    }
  }

  return max_id;
}

///
auto Project::IsEmpty(const Project& project) -> bool {
  return std::all_of(project.diagrams_.begin(), project.diagrams_.end(),
                     [](const auto& diagram) {
                       return diagram.GetNodes().empty() &&
                              diagram.GetLinks().empty();
                     });
}

///
auto Project::FindFamily(const Project& project, core::FamilyId family_id)
    -> IFamily& {
  const auto family = std::find_if(
      project.families_.begin(), project.families_.end(),
      [family_id](const auto& family) { return family->GetId() == family_id; });

  Expects(family != project.families_.end());
  return **family;
}

///
Project::Project(Settings settings,
                 std::vector<std::unique_ptr<IFamily>> families,
                 std::vector<Diagram> diagrams)
    : settings_{std::move(settings)},
      families_{std::move(families)},
      diagrams_{std::move(diagrams)},
      id_generator_{FindMaxId() + 1} {}

///
auto Project::GetIdGenerator() const -> const IdGenerator& {
  // NOLINTNEXTLINE(*-const-cast)
  return const_cast<Project*>(this)->GetIdGenerator();
}

///
auto Project::GetIdGenerator() -> IdGenerator& { return id_generator_; }

///
auto Project::GetFamilies() const
    -> const std::vector<std::unique_ptr<IFamily>>& {
  return families_;
}

///
auto Project::GetDiagrams() const -> const std::vector<Diagram>& {
  // NOLINTNEXTLINE(*-const-cast)
  return const_cast<Project*>(this)->GetDiagrams();
}

///
auto Project::GetDiagrams() -> std::vector<Diagram>& { return diagrams_; }

///
auto Project::EmplaceDiagram(Diagram diagram) -> Diagram& {
  return diagrams_.emplace_back(std::move(diagram));
}

///
void Project::DeleteDiagram(int index) {
  Expects(static_cast<int>(diagrams_.size()) > index);
  diagrams_.erase(diagrams_.begin() + index);
}

///
auto Project::GetSettings() const -> const Settings& {
  // NOLINTNEXTLINE(*-const-cast)
  return const_cast<Project*>(this)->GetSettings();
}

///
auto Project::GetSettings() -> Settings& { return settings_; }
}  // namespace vh::ponc::core