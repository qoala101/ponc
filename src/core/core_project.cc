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

namespace esc::core {
namespace {
///
auto FindMaxId(const std::vector<std::unique_ptr<IFamily>>& families,
               const Diagram& diagram) {
  auto max_id = uintptr_t{1};

  for (const auto& family : families) {
    max_id = std::max(family->GetId().Get(), max_id);
  }

  for (const auto& node : diagram.GetNodes()) {
    max_id = std::max(node->GetId().Get(), max_id);

    for (const auto pin_id : INode::GetAllPinIds(*node)) {
      max_id = std::max(pin_id.Get(), max_id);
    }
  }

  for (const auto& link : diagram.GetLinks()) {
    max_id = std::max(link.id.Get(), max_id);
  }

  return max_id;
}
}  // namespace

///
Project::Project(std::vector<std::unique_ptr<IFamily>> families,
                 Diagram diagram, const Settings& settings)
    : id_generator_{FindMaxId(families, diagram) + 1},
      families_{std::move(families)},
      diagram_{std::move(diagram)},
      settings_{settings} {}

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