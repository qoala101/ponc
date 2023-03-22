#ifndef VH_PONC_COREUI_EMPTY_PIN_TRAITS_H_
#define VH_PONC_COREUI_EMPTY_PIN_TRAITS_H_

#include <imgui_node_editor.h>

#include "coreui_i_pin_traits.h"

namespace vh::ponc::coreui {
///
class EmptyPinTraits : public IPinTraits {
 public:
  ///
  explicit EmptyPinTraits(ne::PinKind pin_kind);

  ///
  auto GetPin() const -> std::variant<ne::PinId, ne::PinKind> override;

 private:
  ///
  ne::PinKind pin_kind_{};
};
}  // namespace vh::ponc::coreui

#endif  // VH_PONC_COREUI_EMPTY_PIN_TRAITS_H_
