#include "coreui_node.h"

#include "cpp_assert.h"

namespace esc::coreui {
auto Node::GetPinRect(ne::PinId pin_id) const -> ImRect {
  const auto pin_rect = pin_rects_.find(pin_id.Get());
  Expects(pin_rect != pin_rects_.end());
  return pin_rect->second;
}

void Node::SetPinRects(std::unordered_map<uintptr_t, ImRect> pin_rects) {
  pin_rects = std::move(pin_rects_);
}
}  // namespace esc::coreui