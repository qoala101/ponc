#include "coreui_flow_pin_drawer.h"

#include "imgui_node_editor.h"

namespace esc::coreui {
FlowPinDrawer::FlowPinDrawer(ne::PinId pin_id) : pin_id_{pin_id} {}

auto FlowPinDrawer::GetPinId() const -> std::optional<ne::PinId> {
  return pin_id_;
}
}  // namespace esc::coreui