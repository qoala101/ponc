#include "core_project.h"

#include <algorithm>
#include <concepts>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include "core_diagram.h"
#include "core_id_generator.h"

namespace vh::ponc::core {
///
auto Project::FindMaxId() const {
  auto max_id = uintptr_t{1};

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
  const auto& diagrams = project.GetDiagrams();

  return std::all_of(diagrams.begin(), diagrams.end(), [](const auto& diagram) {
    return diagram.GetNodes().empty() && diagram.GetLinks().empty();
  });
}

///
Project::Project(const Settings& settings,
                 std::vector<std::unique_ptr<IFamily>> families,
                 std::vector<Diagram> diagrams)
    : settings_{settings},
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
auto Project::GetSettings() const -> const Settings& {
  // NOLINTNEXTLINE(*-const-cast)
  return const_cast<Project*>(this)->GetSettings();
}

///
auto Project::GetSettings() -> Settings& { return settings_; }
}  // namespace vh::ponc::core