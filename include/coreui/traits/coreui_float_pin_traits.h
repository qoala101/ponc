#ifndef VH_PONC_COREUI_FLOAT_PIN_TRAITS_H_
#define VH_PONC_COREUI_FLOAT_PIN_TRAITS_H_

#include <imgui_node_editor.h>

#include "coreui_i_pin_traits.h"

namespace vh::ponc::coreui {
///
class FloatPinTraits : public IPinTraits {
 public:
  ///
  explicit FloatPinTraits(float value);

  ///
  auto GetPin() const -> std::variant<ne::PinId, ne::PinKind> override;
  ///
  auto GetValue() const -> PinValueVariant override;

 private:
  ///
  float value_{};
};
}  // namespace vh::ponc::coreui

#endif  // VH_PONC_COREUI_FLOAT_PIN_TRAITS_H_
