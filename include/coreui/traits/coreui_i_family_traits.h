#ifndef VH_PONC_COREUI_I_FAMILY_TRAITS_H_
#define VH_PONC_COREUI_I_FAMILY_TRAITS_H_

#include <string>

#include "cpp_interface.h"

namespace vh::ponc::coreui {
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
}  // namespace vh::ponc::coreui

#endif  // VH_PONC_COREUI_I_FAMILY_TRAITS_H_
