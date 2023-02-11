#include "draw_flow_input_pin_drawer.h"

namespace esc::draw {
auto FlowInputPinDrawer::GetLabel() const -> std::string { return {}; }

auto FlowInputPinDrawer::GetKind() const -> ne::PinKind {
  return ne::PinKind::Input;
}

auto FlowInputPinDrawer::GetFloat() -> float* { return &value_; }

auto FlowInputPinDrawer::IsEditable() const -> bool { return false; }
}  // namespace esc::draw