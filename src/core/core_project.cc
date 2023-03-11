#include "core_project.h"

#include <bits/stdc++.h>

#include <algorithm>
#include <concepts>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include "core_diagram.h"
#include "core_id_generator.h"
#include "cpp_assert.h"

namespace esc::core {
///
auto Project::FindMaxId() const {
  auto max_id = uintptr_t{1};

  for (const auto& family : families_) {
    max_id = std::max(family->GetId().Get(), max_id);
  }

  for (const auto& node : diagram_.GetNodes()) {
    max_id = std::max(node->GetId().Get(), max_id);

    for (const auto& pin : INode::GetAllPins(*node)) {
      max_id = std::max(pin.first.Get(), max_id);
    }
  }

  for (const auto& link : diagram_.GetLinks()) {
    max_id = std::max(link.id.Get(), max_id);
  }

  return max_id;
}

///
auto Project::GetDefaultFamily(
    const std::vector<std::unique_ptr<IFamily>>& families,
    FamilyType family_type) -> const IFamily& {
  const auto family = std::find_if(families.begin(), families.end(),
                                   [family_type](const auto& family) {
                                     return family->GetType() == family_type;
                                   });
  Expects(family != families.end());
  return **family;
}

///
Project::Project(const Settings& settings,
                 std::vector<std::unique_ptr<IFamily>> families,
                 Diagram diagram)
    : settings_{settings},
      families_{std::move(families)},
      diagram_{std::move(diagram)},
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
auto Project::GetDiagram() const -> const Diagram& {
  // NOLINTNEXTLINE(*-const-cast)
  return const_cast<Project*>(this)->GetDiagram();
}

///
auto Project::GetDiagram() -> Diagram& { return diagram_; }

///
auto Project::GetSettings() const -> const Settings& {
  // NOLINTNEXTLINE(*-const-cast)
  return const_cast<Project*>(this)->GetSettings();
}

///
auto Project::GetSettings() -> Settings& { return settings_; }
}  // namespace esc::core