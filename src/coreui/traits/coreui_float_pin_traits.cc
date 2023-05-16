#include "coreui_float_pin_traits.h"

#include "coreui_i_pin_traits.h"
#include "imgui_node_editor.h"

namespace vh::ponc::coreui {
///
FloatPinTraits::FloatPinTraits(float value) : value_{value} {}

///
auto FloatPinTraits::GetPin() const -> std::variant<ne::PinId, ne::PinKind> {
  return ne::PinKind::Input;
}

///
auto FloatPinTraits::GetValue() const -> PinValueVariant { return value_; }
}  // namespace vh::ponc::coreui