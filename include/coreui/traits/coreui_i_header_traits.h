#ifndef VH_PONC_COREUI_I_HEADER_TRAITS_H_
#define VH_PONC_COREUI_I_HEADER_TRAITS_H_

#include <string>

#include "cpp_interface.h"
#include "imgui.h"

namespace vh::ponc::coreui {
///
class IHeaderTraits : public cpp::Interface {
 public:
  ///
  virtual auto GetColor() const -> ImColor = 0;
};
}  // namespace vh::ponc::coreui

#endif  // VH_PONC_COREUI_I_HEADER_TRAITS_H_
