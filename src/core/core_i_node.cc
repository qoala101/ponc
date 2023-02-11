#include "core_i_node.h"

#include <bits/ranges_algo.h>

#include <vector>

namespace esc::core {
INode::INode(ne::NodeId id, std::vector<ne::PinId> pin_ids)
    : id_{id}, pin_ids_{std::move(pin_ids)} {}

void INode::OnFrame(const State& /*unused*/) {}

auto INode::GetId() const -> ne::NodeId { return id_; }

auto INode::GetPinIds() const -> const std::vector<ne::PinId>& {
  return pin_ids_;
}

auto INode::GetPosition() const -> ImVec2 { return ne::GetNodePosition(id_); }

void INode::SetPosition(const ImVec2& position) {
  ne::SetNodePosition(id_, position);
}

auto INode::GetPinIndex(ne::PinId pin_id) const -> int {
  return static_cast<int>(std::ranges::find(pin_ids_, pin_id) -
                          pin_ids_.begin());
}
}  // namespace esc::core