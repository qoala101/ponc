#include "coreui_flow_input_pin_drawer.h"

#include "cpp_assert.h"
#include "flow_node_flow.h"

namespace esc::coreui {
FlowInputPinDrawer::FlowInputPinDrawer(const flow::NodeFlow& flow)
    : value_{(Expects(flow.input_pin_flow.has_value()),
              flow.input_pin_flow->second)} {}

auto FlowInputPinDrawer::GetLabel() const -> std::string { return {}; }

auto FlowInputPinDrawer::GetKind() const -> ne::PinKind {
  return ne::PinKind::Input;
}

auto FlowInputPinDrawer::GetFloat() -> float* { return &value_; }

auto FlowInputPinDrawer::IsEditable() const -> bool { return false; }
}  // namespace esc::coreui