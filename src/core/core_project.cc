/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "core_project.h"

#include <imgui_node_editor.h>

#include <algorithm>
#include <exception>
#include <string>
#include <utility>
#include <vector>

#include "core_area.h"
#include "core_connection.h"
#include "core_diagram.h"
#include "core_i_node.h"
#include "core_id_generator.h"
#include "core_id_value.h"
#include "core_link.h"
#include "cpp_assert.h"

namespace vh::ponc::core {
///
auto Project::FindMaxId() const {
  auto max_id = UnspecifiedIdValue{1};

  for (const auto& family : families_) {
    max_id = std::max(family->GetId().Get(), max_id);
  }

  for (const auto& connection : connections_) {
    max_id = std::max(connection.id.Get(), max_id);
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

    for (const auto& area : diagram.GetAreas()) {
      max_id = std::max(area.id.Get(), max_id);
    }
  }

  return max_id;
}

///
auto Project::IsEmpty(const Project& project) -> bool {
  return std::all_of(project.diagrams_.cbegin(), project.diagrams_.cend(),
                     [](const auto& diagram) {
                       return diagram.GetNodes().empty() &&
                              diagram.GetLinks().empty() &&
                              diagram.GetAreas().empty();
                     });
}

///
auto Project::FindFamily(const Project& project, FamilyId family_id)
    -> IFamily& {
  const auto family = std::find_if(
      project.families_.cbegin(), project.families_.cend(),
      [family_id](const auto& family) { return family->GetId() == family_id; });

  Expects(family != project.families_.cend());
  return **family;
}

///
Project::Project(Settings settings,
                 std::vector<std::unique_ptr<IFamily>> families,
                 std::vector<Connection> connections,
                 std::vector<Diagram> diagrams)
    : settings_{std::move(settings)},
      families_{std::move(families)},
      connections_{std::move(connections)},
      diagrams_{std::move(diagrams)},
      id_generator_{FindMaxId() + 1} {}

///
auto Project::GetSettings() const -> const Settings& {
  // NOLINTNEXTLINE(*-const-cast)
  return const_cast<Project*>(this)->GetSettings();
}

///
auto Project::GetSettings() -> Settings& { return settings_; }

///
auto Project::GetFamilies() const
    -> const std::vector<std::unique_ptr<IFamily>>& {
  return families_;
}

///
auto Project::GetConnections() const -> const std::vector<Connection>& {
  // NOLINTNEXTLINE(*-const-cast)
  return const_cast<Project*>(this)->GetConnections();
}

///
auto Project::GetConnections() -> std::vector<Connection>& {
  return connections_;
}

///
auto Project::EmplaceConnection(Connection connection) -> Connection& {
  return connections_.emplace_back(std::move(connection));
}

///
void Project::DeleteConnection(ConnectionId connection_id) {
  std::erase_if(connections_, [connection_id](const auto& connection) {
    return connection.id == connection_id;
  });
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
  diagrams_.erase(diagrams_.cbegin() + index);
}

///
auto Project::GetIdGenerator() const -> const IdGenerator& {
  // NOLINTNEXTLINE(*-const-cast)
  return const_cast<Project*>(this)->GetIdGenerator();
}

///
auto Project::GetIdGenerator() -> IdGenerator& { return id_generator_; }
}  // namespace vh::ponc::core