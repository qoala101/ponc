/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include <imgui_node_editor.h>

#include <variant>

#include "coreui_empty_pin_traits.h"

namespace vh::ponc::coreui {
///
EmptyPinTraits::EmptyPinTraits(ne::PinKind pin_kind) : pin_kind_{pin_kind} {}

///
auto EmptyPinTraits::GetPin() const -> std::variant<ne::PinId, ne::PinKind> {
  return pin_kind_;
}
}  // namespace vh::ponc::coreui