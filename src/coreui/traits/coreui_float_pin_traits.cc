/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "coreui_float_pin_traits.h"

#include <imgui_node_editor.h>

#include "coreui_i_pin_traits.h"

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