#ifndef VH_COREUI_I_FAMILY_TRAITS_H_
#define VH_COREUI_I_FAMILY_TRAITS_H_

#include <string>

#include "core_i_family.h"
#include "cpp_interface.h"
#include "imgui.h"

namespace esc::coreui {
///
class IFamilyTraits : public cpp::Interface {
 public:
  ///
  static auto GroupByLabels(
      const std::vector<std::unique_ptr<core::IFamily>> &families)
      -> std::vector<std::pair<std::string, std::vector<core::IFamily *>>>;

  ///
  virtual auto GetLabel() const -> std::string = 0;
  ///
  virtual auto GetColor() const -> ImColor = 0;
  ///
  virtual auto GetGroupLabel() const -> std::string;
  ///
  virtual auto IsUserAccessible() -> bool;
};
}  // namespace esc::coreui

#endif  // VH_COREUI_I_FAMILY_TRAITS_H_
