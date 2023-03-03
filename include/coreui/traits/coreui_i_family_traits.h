#ifndef VH_COREUI_I_FAMILY_TRAITS_H_
#define VH_COREUI_I_FAMILY_TRAITS_H_

#include <string>

#include "cpp_interface.h"

namespace esc::coreui {
///
class IFamilyTraits : public cpp::Interface {
 public:
  ///
  virtual auto GetLabel() const -> std::string = 0;
  ///
  virtual auto GetGroupLabel() const -> std::string;
  ///
  virtual auto IsUserAccessible() -> bool;
};
}  // namespace esc::coreui

#endif  // VH_COREUI_I_FAMILY_TRAITS_H_
