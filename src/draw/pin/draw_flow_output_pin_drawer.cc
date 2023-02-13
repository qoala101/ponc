#include "draw_flow_output_pin_drawer.h"

#include "cpp_assert.h"

namespace esc::draw {
FlowOutputPinDrawer::FlowOutputPinDrawer(const core::Flow& flow,
                                         ne::PinId pin_id)
    : value_{(cpp::Expects(flow.output_pin_flows.contains(pin_id.Get())),
              flow.output_pin_flows.at(pin_id.Get()))} {}

auto FlowOutputPinDrawer::GetLabel() const -> std::string { return {}; }

auto FlowOutputPinDrawer::GetKind() const -> ne::PinKind {
  return ne::PinKind::Output;
}

auto FlowOutputPinDrawer::GetFloat() -> float* { return &value_; }

auto FlowOutputPinDrawer::IsEditable() const -> bool { return false; }
}  // namespace esc::draw