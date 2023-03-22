#include "coreui_flow_pin_traits.h"

#include "coreui_i_pin_traits.h"
#include "imgui_node_editor.h"

namespace vh::ponc::coreui {
///
FlowPinTraits::FlowPinTraits(ne::PinId pin_id) : pin_id_{pin_id} {}

///
auto FlowPinTraits::GetPin() const -> std::variant<ne::PinId, ne::PinKind> {
  return pin_id_;
}

///
auto FlowPinTraits::GetValue() const -> PinValueVariant { return PinFlow{}; }
}  // namespace vh::ponc::coreui