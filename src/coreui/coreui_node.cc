#include "coreui_node.h"

#include "cpp_assert.h"
#include "imgui.h"

namespace esc::coreui {
///
Node::Node(cpp::SafePtr<core::INode> node, NodeData data)
    : node_{std::move(node)}, data_{std::move(data)} {}

///
auto Node::GetNode() const -> core::INode& { return *node_; }

///
auto Node::GetData() const -> const NodeData& { return data_; }

///
auto Node::GetSize() const -> const ImVec2& { return size_; }

///
void Node::SetSize(const ImVec2& size) { size_ = size; }

///
auto Node::GetPinTipPos(ne::PinId pin_id) const -> ImVec2 {
  const auto pin_id_value = pin_id.Get();
  Expects(pin_tip_poses_.contains(pin_id_value));
  return pin_tip_poses_.at(pin_id_value);
}

///
void Node::SetPinTipPos(ne::PinId pin_id, const ImVec2& pos) {
  pin_tip_poses_.emplace(pin_id, pos);
}
}  // namespace esc::coreui