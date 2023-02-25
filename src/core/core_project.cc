#include "core_project.h"

#include "core_diagram.h"
#include "core_id_generator.h"

namespace esc::core {
namespace {
auto FindMaxId(const std::vector<std::shared_ptr<IFamily>>& families,
               const Diagram& diagram) {
  auto max_id = uintptr_t{1};

  for (const auto& family : families) {
    max_id = std::max(family->GetId().Get(), max_id);
  }

  for (const auto& node : diagram.GetNodes()) {
    max_id = std::max(node->GetId().Get(), max_id);

    for (const auto pin_id : core::GetAllPinIds(*node)) {
      max_id = std::max(pin_id.Get(), max_id);
    }
  }

  for (const auto& link : diagram.GetLinks()) {
    max_id = std::max(link.id.Get(), max_id);
  }

  return max_id;
}
}  // namespace

Project::Project(std::vector<std::shared_ptr<IFamily>> families,
                 Diagram diagram, const Settings& settings)
    : id_generator_{FindMaxId(families, diagram) + 1},
      families_{std::move(families)},
      diagram_{std::move(diagram)},
      settings_{settings} {
  if (!families_.empty()) {
    return;
  }

  // auto free_pin_family = std::make_shared<FreePinFamily>();
  // free_pin_family_ = free_pin_family;
  // families_.emplace_back(std::move(free_pin_family));

  // auto placeholder_family = std::make_shared<PlaceholderFamily>();
  // placeholder_family_ = placeholder_family;
  // families_.emplace_back(std::move(placeholder_family));
}

auto Project::GetIdGenerator() const -> const IdGenerator& {
  // NOLINTNEXTLINE(*-const-cast)
  return const_cast<Project*>(this)->GetIdGenerator();
}

auto Project::GetIdGenerator() -> IdGenerator& { return id_generator_; }

auto Project::GetFamilies() const
    -> const std::vector<std::shared_ptr<IFamily>>& {
  return families_;
}

// // auto Project::GetFreePinFamily() const -> FreePinFamily& {
//   auto lock = free_pin_family_.lock();
//   Expects(lock != nullptr);
//   return *lock;
// }

// // auto Project::GetPlaceholderFamily() const -> PlaceholderFamily& {
//   auto lock = placeholder_family_.lock();
//   Expects(lock != nullptr);
//   return *lock;
// }

auto Project::GetDiagram() const -> const Diagram& {
  // NOLINTNEXTLINE(*-const-cast)
  return const_cast<Project*>(this)->GetDiagram();
}

auto Project::GetDiagram() -> Diagram& { return diagram_; }

auto Project::GetSettings() const -> const Settings& {
  // NOLINTNEXTLINE(*-const-cast)
  return const_cast<Project*>(this)->GetSettings();
}

auto Project::GetSettings() -> Settings& { return settings_; }
}  // namespace esc::core