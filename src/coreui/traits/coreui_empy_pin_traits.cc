#include <imgui_node_editor.h>

#include "coreui_empty_pin_traits.h"

namespace esc::coreui {
///
EmptyPinTraits::EmptyPinTraits(ne::PinKind pin_kind) : pin_kind_{pin_kind} {}

///
auto EmptyPinTraits::GetPin() const -> std::variant<ne::PinId, ne::PinKind> {
  return pin_kind_;
}
}  // namespace esc::coreui