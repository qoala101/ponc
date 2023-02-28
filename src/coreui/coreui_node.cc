#include "coreui_node.h"

#include "cpp_assert.h"

namespace esc::coreui {
///
Node::Node(NodeData data) : data_{std::move(data)} {}

///
auto Node::GetPinRect(ne::PinId pin_id) const -> ImRect {
  const auto pin_rect = pin_rects_.find(pin_id.Get());
  Expects(pin_rect != pin_rects_.end());
  return pin_rect->second;
}

///
void Node::SetPinRect(ne::PinId pin_id, ImRect rect) {
  pin_rects_[pin_id.Get()] = rect;
}
}  // namespace esc::coreui