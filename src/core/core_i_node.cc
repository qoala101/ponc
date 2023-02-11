#include "core_i_node.h"

namespace esc::core {
INode::INode(ne::NodeId id, std::vector<ne::PinId> pin_ids)
    : id_{id}, pin_ids_{std::move(pin_ids)} {}

auto INode::GetId() const -> ne::NodeId { return id_; }

auto INode::GetPinIds() const -> const std::vector<ne::PinId>& {
  return pin_ids_;
}

auto INode::GetPosition() const -> ImVec2 { return ne::GetNodePosition(id_); }

void INode::SetPosition(const ImVec2& position) {
  ne::SetNodePosition(id_, position);
}
}  // namespace esc::core