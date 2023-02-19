/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_CORE_PROJECT_H_
#define VH_CORE_PROJECT_H_

#include "core_diagram.h"
// #include "core_free_pin_family.h"
#include "core_i_family.h"
// #include "core_placeholder_family.h"
#include "core_settings.h"

namespace esc::core {
// ---
class Project {
 public:
  // ---
  explicit Project(std::vector<std::shared_ptr<IFamily>> families = {},
                   Diagram diagram = Diagram{}, const Settings &settings = {});

  // ---
  auto GetFamilies [[nodiscard]] () const
      -> const std::vector<std::shared_ptr<IFamily>> &;
  // // ---
  // auto GetFreePinFamily [[nodiscard]] () const -> FreePinFamily &;
  // // ---
  // auto GetPlaceholderFamily [[nodiscard]] () const -> PlaceholderFamily &;

  // ---
  auto GetDiagram [[nodiscard]] () const -> const Diagram &;
  // ---
  auto GetDiagram [[nodiscard]] () -> Diagram &;

 private:
  // ---
  std::vector<std::shared_ptr<IFamily>> families_{};
  // // ---
  // std::weak_ptr<FreePinFamily> free_pin_family_{};
  // // ---
  // std::weak_ptr<PlaceholderFamily> placeholder_family_{};
  // ---
  Diagram diagram_{};
  // ---
  Settings settings_{};
};
}  // namespace esc::core

#endif  // VH_CORE_PROJECT_H_
