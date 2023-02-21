/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#include "core_project.h"

#include "core_diagram.h"

namespace esc::core {
// ---
Project::Project(std::vector<std::shared_ptr<IFamily>> families,
                 Diagram diagram, const Settings& settings)
    : families_{std::move(families)},
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

// ---
auto Project::GetFamilies() const
    -> const std::vector<std::shared_ptr<IFamily>>& {
  return families_;
}

// // ---
// auto Project::GetFreePinFamily() const -> FreePinFamily& {
//   auto lock = free_pin_family_.lock();
//   Expects(lock != nullptr);
//   return *lock;
// }

// // ---
// auto Project::GetPlaceholderFamily() const -> PlaceholderFamily& {
//   auto lock = placeholder_family_.lock();
//   Expects(lock != nullptr);
//   return *lock;
// }

// ---
auto Project::GetDiagram() const -> const Diagram& {
  // NOLINTNEXTLINE(*-const-cast)
  return const_cast<Project*>(this)->GetDiagram();
}

// ---
auto Project::GetDiagram() -> Diagram& { return diagram_; }

// ---
auto Project::GetSettings() const -> const Settings& {
  // NOLINTNEXTLINE(*-const-cast)
  return const_cast<Project*>(this)->GetSettings();
}

// ---
auto Project::GetSettings() -> Settings& { return settings_; }
}  // namespace esc::core