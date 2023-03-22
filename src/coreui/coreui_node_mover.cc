#include <functional>
#include <iostream>

#include "core_diagram.h"
#include "cpp_assert.h"
#define IMGUI_DEFINE_MATH_OPERATORS

#include "core_i_node.h"
#include "coreui_diagram.h"
#include "coreui_node_mover.h"

namespace vh::ponc::coreui {
///
NodeMover::NodeMover(cpp::SafePtr<Diagram> parent_diagram)
    : parent_diagram_{std::move(parent_diagram)} {}

///
void NodeMover::OnFrame() {
  MoveNewNodes();
  ApplyMoves();

  nodes_to_move_.clear();
  node_sizes_.clear();
  pin_poses_.clear();
}

///
void NodeMover::MoveNode(ne::NodeId node_id) {
  nodes_to_move_.insert(node_id.Get());
}

///
void NodeMover::MoveNodesTo(const std::vector<ne::NodeId>& node_ids,
                            ImVec2 pos) {
  const auto& diagram = parent_diagram_->GetDiagram();

  for (const auto node_id : node_ids) {
    auto& node = core::Diagram::FindNode(diagram, node_id);
    node.SetPos(pos);
    MoveNode(node_id);

    pos.y += GetNodeSize(node_id).y;
  }
}

///
void NodeMover::MovePinTo(ne::PinId pin_id, const ImVec2& pos) {
  const auto current_pin_pos = GetPinPos(pin_id);

  auto& node =
      core::Diagram::FindPinNode(parent_diagram_->GetDiagram(), pin_id);
  const auto matching_node_pos = node.GetPos() - current_pin_pos + pos;

  node.SetPos(matching_node_pos);
  MoveNode(node.GetId());
}

///
auto NodeMover::GetNodeSize(ne::NodeId node_id) const -> const ImVec2& {
  const auto node_id_value = node_id.Get();
  Expects(node_sizes_.contains(node_id_value));
  return node_sizes_.at(node_id_value);
}

///
void NodeMover::SetNodeSize(ne::NodeId node_id, const ImVec2& size) {
  node_sizes_.emplace(node_id, size);
}

///
auto NodeMover::GetPinPos(ne::PinId pin_id) const -> const ImVec2& {
  const auto pin_id_value = pin_id.Get();
  Expects(pin_poses_.contains(pin_id_value));
  return pin_poses_.at(pin_id_value);
}

///
void NodeMover::SetPinPos(ne::PinId pin_id, const ImVec2& pos) {
  pin_poses_.emplace(pin_id, pos);
}

///
void NodeMover::MoveNewNodes() {
  const auto& diagram = parent_diagram_->GetDiagram();

  for (const auto& node : diagram.GetNodes()) {
    const auto node_id = node->GetId();

    if (const auto node_is_new = !node_sizes_.contains(node_id.Get())) {
      MoveNode(node_id);
    }
  }
}

///
void NodeMover::ApplyMoves() const {
  const auto& diagram = parent_diagram_->GetDiagram();

  for (const auto node_id : nodes_to_move_) {
    ne::SetNodePosition(node_id,
                        core::Diagram::FindNode(diagram, node_id).GetPos());
  }
}
}  // namespace vh::ponc::coreui