#include <algorithm>
#include <functional>
#include <iostream>
#include <numeric>
#include <unordered_map>
#include <vector>

#include "core_diagram.h"
#include "core_id_value.h"
#include "coreui_flow_tree_node.h"
#include "cpp_assert.h"
#include "imgui_node_editor.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "core_i_node.h"
#include "coreui_diagram.h"
#include "coreui_i_node_traits.h"
#include "coreui_node_mover.h"
#include "flow_tree_node.h"
#include "flow_tree_traversal.h"
#include "imgui.h"

namespace vh::ponc::coreui {
///
NodeMover::NodeMover(cpp::SafePtr<Diagram> parent_diagram,
                     cpp::SafePtr<core::Settings> settings)
    : parent_diagram_{std::move(parent_diagram)},
      settings_{std::move(settings)} {}

///
void NodeMover::OnFrame() {
  MarkNewNodesToMove();
  ApplyMoves();

  nodes_to_move_.clear();
  node_sizes_.clear();
  pin_poses_.clear();
}

///
void NodeMover::MoveNodeTo(ne::NodeId node_id, const ImVec2& pos) {
  const auto& diagram = parent_diagram_->GetDiagram();
  auto& node = core::Diagram::FindNode(diagram, node_id);

  MoveNodePinPoses(node, pos);
  node.SetPos(pos);
  MarkToMove(node_id);
}

///
auto NodeMover::DoNodesNeedSpacing(ne::NodeId first_node,
                                   ne::NodeId second_node) const {
  const auto node_ids = std::array{first_node, second_node};
  const auto& diagram = parent_diagram_->GetDiagram();

  return std::any_of(
      node_ids.cbegin(), node_ids.cend(), [&diagram](const auto node_id) {
        const auto& node = core::Diagram::FindNode(diagram, node_id);
        const auto node_has_header =
            node.CreateUiTraits()->CreateHeaderTraits().has_value();
        return node_has_header;
      });
}

///
void NodeMover::ArrangeVerticallyAt(const std::vector<ne::NodeId>& node_ids,
                                    const ImVec2& pos) {
  auto next_node_pos = pos;

  for (auto node_id = node_ids.cbegin(); node_id != node_ids.end(); ++node_id) {
    MoveNodeTo(*node_id, next_node_pos);
    next_node_pos.y += GetNodeSize(*node_id).y;

    const auto next_id = std::next(node_id);

    if ((next_id != node_ids.end()) && DoNodesNeedSpacing(*node_id, *next_id)) {
      next_node_pos.y +=
          static_cast<float>(settings_->arrange_vertical_spacing);
    }
  }
}

///
auto NodeMover::GetNodePos(ne::NodeId node_id) const {
  const auto& diagram = parent_diagram_->GetDiagram();
  const auto& node = core::Diagram::FindNode(diagram, node_id);
  return node.GetPos();
}

///
auto NodeMover::GetNodeRect(ne::NodeId node_id) const {
  auto rect = ImRect{{}, GetNodeSize(node_id)};
  rect.Translate(GetNodePos(node_id));
  return rect;
}

///
auto NodeMover::GetTreeRect(const flow::TreeNode& tree_node) const {
  auto rect = GetNodeRect(tree_node.node_id);

  flow::TraverseDepthFirst(
      tree_node,
      [this, &rect](const auto& tree_node) {
        rect.Add(GetNodeRect(tree_node.node_id));
      },
      [](const auto&) {});

  return rect;
}

///
auto NodeMover::GetOtherPinPos(ne::PinId pin_id) const -> const ImVec2& {
  const auto& diagram = parent_diagram_->GetDiagram();

  const auto link = core::Diagram::FindPinLink(diagram, pin_id);
  Expects(link.has_value());

  const auto other_pin = core::Link::GetOtherPin(**link, pin_id);
  return GetPinPos(other_pin);
}

///
auto NodeMover::GetTakenPinsRect(const flow::TreeNode& tree_node) const
    -> std::optional<ImRect> {
  if (tree_node.child_nodes.empty()) {
    return std::nullopt;
  }

  auto rect = std::optional<ImRect>{};

  for (const auto& [pin_id, child_node] : tree_node.child_nodes) {
    const auto pin_pos = GetPinPos(pin_id);

    if (rect.has_value()) {
      rect->Add(pin_pos);
      continue;
    }

    rect.emplace(pin_pos, pin_pos);
  }

  return rect;
}

///
auto NodeMover::DoesChildNeedSpacing(
    const flow::TreeNode& tree_node,
    decltype(flow::TreeNode::child_nodes)::const_iterator child_node) const {
  const auto next_node = std::next(child_node);

  if (next_node == tree_node.child_nodes.cend()) {
    return false;
  }

  if (!child_node->second.child_nodes.empty()) {
    return true;
  }

  return DoNodesNeedSpacing(child_node->second.node_id,
                            next_node->second.node_id);
}

///
auto NodeMover::GetChildrenNeedingSpacing(
    const flow::TreeNode& tree_node) const {
  auto children_needing_spacing =
      std::unordered_set<core::IdValue<ne::NodeId>>{};

  for (auto child_node = tree_node.child_nodes.cbegin();
       child_node != tree_node.child_nodes.cend(); ++child_node) {
    if (DoesChildNeedSpacing(tree_node, child_node)) {
      children_needing_spacing.emplace(child_node->second.node_id);
    }
  }

  return children_needing_spacing;
}

///
void NodeMover::ArrangeAsTreeVisitNode(const flow::TreeNode& tree_node) {
  const auto& child_nodes = tree_node.child_nodes;

  if (child_nodes.empty()) {
    return;
  }

  const auto node_rect = GetNodeRect(tree_node.node_id);
  const auto next_child_x =
      node_rect.Max.x +
      static_cast<float>(settings_->arrange_horizontal_spacing);

  const auto children_needing_spacing = GetChildrenNeedingSpacing(tree_node);
  const auto vertical_spacing =
      static_cast<float>(settings_->arrange_vertical_spacing);

  const auto& [first_output_pin, first_child] = *child_nodes.cbegin();
  const auto tree_top_to_first_input_pin_distance =
      GetOtherPinPos(first_output_pin).y - GetTreeRect(first_child).Min.y;

  const auto& [last_output_pin, last_child] = *std::prev(child_nodes.cend());
  const auto last_input_pin_to_tree_bot_distance =
      GetTreeRect(last_child).Max.y - GetOtherPinPos(last_output_pin).y;

  const auto direct_children_height = std::accumulate(
      child_nodes.cbegin(), child_nodes.cend(),
      vertical_spacing * static_cast<float>(children_needing_spacing.size()) -
          tree_top_to_first_input_pin_distance -
          last_input_pin_to_tree_bot_distance,
      [this](const auto height, const auto& child_node) {
        return height + GetTreeRect(child_node.second).GetHeight();
      });

  const auto taken_pins_rect = GetTakenPinsRect(tree_node);
  Expects(taken_pins_rect.has_value());

  auto next_child_y = taken_pins_rect->GetCenter().y -
                      tree_top_to_first_input_pin_distance -
                      direct_children_height / 2;

  for (const auto& child_node : child_nodes) {
    MoveTreeTo(child_node.second, {next_child_x, next_child_y});

    const auto child_tree_rect = GetTreeRect(child_node.second);
    next_child_y += child_tree_rect.GetHeight();

    if (children_needing_spacing.contains(child_node.second.node_id.Get())) {
      next_child_y += vertical_spacing;
    }
  }
}

///
void NodeMover::ArrangeAsTreeImpl(const flow::TreeNode& tree_node) {
  flow::TraverseDepthFirst(
      tree_node, [](const auto&) {},
      std::bind_front(&NodeMover::ArrangeAsTreeVisitNode, this));
}

///
void NodeMover::ArrangeAsTree(const flow::TreeNode& tree_node) {
  ArrangeAsTreeImpl(tree_node);
}

///
void NodeMover::ArrangeAsTrees(const std::vector<flow::TreeNode>& tree_nodes) {
  auto last_tree_rect = std::optional<ImRect>{};

  for (const auto& tree_node : tree_nodes) {
    ArrangeAsTreeImpl(tree_node);
    const auto tree_rect = GetTreeRect(tree_node);

    if (!last_tree_rect.has_value()) {
      last_tree_rect = tree_rect;
      continue;
    }

    MoveTreeTo(tree_node,
               {last_tree_rect->Min.x,
                last_tree_rect->Max.y +
                    static_cast<float>(settings_->arrange_vertical_spacing)});
    last_tree_rect = GetTreeRect(tree_node);
  }
}

void NodeMover::MoveTreesToRightOf(
    const std::vector<flow::TreeNode>& fixed_trees,
    const std::vector<flow::TreeNode>& moved_trees) {
  const auto& child_nodes = moved_trees;

  if (fixed_trees.empty() || child_nodes.empty()) {
    return;
  }

  auto rect = GetNodeRect(fixed_trees[0].node_id);
  auto old_node_poses = std::unordered_map<core::IdValue<ne::NodeId>, ImVec2>{};
  auto output_tree_parents = std::vector<flow::TreeNode>{};

  for (const auto& tree_node : fixed_trees) {
    auto traversing_moved_tree_id = std::optional<ne::NodeId>{};

    flow::TraverseDepthFirst(
        tree_node,
        [this, &moved_trees, &rect, &old_node_poses, &traversing_moved_tree_id,
         &output_tree_parents](const auto& tree_node) {
          auto tree_copy = tree_node;
          tree_copy.child_nodes.clear();

          for (const auto& child : tree_node.child_nodes) {
            if (std::any_of(moved_trees.cbegin(), moved_trees.cend(),
                            [&child](const auto& moved_tree) {
                              return moved_tree.node_id == child.second.node_id;
                            })) {
              tree_copy.child_nodes.emplace(child);
            }
          }

          if (!tree_copy.child_nodes.empty()) {
            output_tree_parents.emplace_back(std::move(tree_copy));
          }

          if (traversing_moved_tree_id.has_value()) {
            return;
          }

          if (std::any_of(moved_trees.cbegin(), moved_trees.cend(),
                          [&tree_node](const auto& moved_tree) {
                            return moved_tree.node_id == tree_node.node_id;
                          })) {
            traversing_moved_tree_id = tree_node.node_id;
            return;
          }

          rect.Add(GetNodeRect(tree_node.node_id));
          old_node_poses.emplace(tree_node.node_id,
                                 GetNodePos(tree_node.node_id));
        },
        [&traversing_moved_tree_id](const auto& tree_node) {
          if (traversing_moved_tree_id.has_value() &&
              (tree_node.node_id == *traversing_moved_tree_id)) {
            traversing_moved_tree_id.reset();
          }
        });
  }

  const auto node_rect = rect;
  const auto next_child_x =
      node_rect.Max.x +
      static_cast<float>(settings_->arrange_horizontal_spacing);

  auto comp = [this](const flow::TreeNode& left, const flow::TreeNode& right) {
    Expects(!left.child_nodes.empty());
    const auto first_left_pin = left.child_nodes.cbegin()->first;

    Expects(!right.child_nodes.empty());
    const auto first_right_pin = right.child_nodes.cbegin()->first;

    return GetPinPos(first_left_pin).y < GetPinPos(first_right_pin).y;
  };

  std::stable_sort(output_tree_parents.begin(), output_tree_parents.end(),
                   comp);

  ArrangeAsTrees(output_tree_parents);

  for (const auto& [node_id, node_pos] : old_node_poses) {
    MoveNodeTo(node_id, node_pos);
  }

  const auto vertical_spacing =
      static_cast<float>(settings_->arrange_vertical_spacing);

  const auto& [first_output_pin, first_child] =
      *output_tree_parents.front().child_nodes.cbegin();
  const auto tree_top_to_first_input_pin_distance =
      GetOtherPinPos(first_output_pin).y - GetTreeRect(first_child).Min.y;

  // std::cout << GetNodePos(480).y << " " << GetPinPos(481).y << "\n";

  const auto [last_output_pin, last_child] =
      *std::prev(output_tree_parents.back().child_nodes.cend());
  const auto last_input_pin_to_tree_bot_distance =
      GetTreeRect(last_child).Max.y - GetOtherPinPos(last_output_pin).y;

  auto direct_children_height =
      vertical_spacing * (static_cast<float>(output_tree_parents.size()) - 1) -
      tree_top_to_first_input_pin_distance -
      last_input_pin_to_tree_bot_distance;

  for (const auto& output_tree_parent : output_tree_parents) {
    Expects(!output_tree_parent.child_nodes.empty());
    const auto first_child = output_tree_parent.child_nodes.cbegin();
    const auto last_child = std::prev(output_tree_parent.child_nodes.cend());
    direct_children_height += GetTreeRect(last_child->second).Max.y -
                              GetTreeRect(first_child->second).Min.y;
  }

  auto taken_pins_rect = std::optional<ImRect>{};

  for (const auto& output_tree_parent : output_tree_parents) {
    for (const auto& [pin_id, child_node] : output_tree_parent.child_nodes) {
      const auto pin_pos = GetPinPos(pin_id);

      if (taken_pins_rect.has_value()) {
        taken_pins_rect->Add(pin_pos);
        continue;
      }

      taken_pins_rect.emplace(pin_pos, pin_pos);
    }
  }

  std::cout << tree_top_to_first_input_pin_distance << " "
            << direct_children_height << " " << taken_pins_rect->GetHeight()
            << "\n";

  auto next_child_y = taken_pins_rect->GetCenter().y -
                      tree_top_to_first_input_pin_distance -
                      direct_children_height / 2;

  for (const auto& output_tree_parent : output_tree_parents) {
    Expects(!output_tree_parent.child_nodes.empty());
    const auto first_child = output_tree_parent.child_nodes.cbegin();
    const auto shift = ImVec2{next_child_x, next_child_y} -
                       GetTreeRect(first_child->second).Min;

    for (const auto& [pin_id, output_tree] : output_tree_parent.child_nodes) {
      MoveTreeTo(output_tree, GetTreeRect(output_tree).Min + shift);
    }

    const auto last_child = std::prev(output_tree_parent.child_nodes.cend());
    next_child_y = GetTreeRect(last_child->second).Max.y +
                   static_cast<float>(settings_->arrange_vertical_spacing);
  }
}

///
void NodeMover::MovePinTo(ne::PinId pin_id, const ImVec2& pos) {
  const auto current_pin_pos = GetPinPos(pin_id);
  const auto& node =
      core::Diagram::FindPinNode(parent_diagram_->GetDiagram(), pin_id);
  const auto matching_node_pos = node.GetPos() - current_pin_pos + pos;

  MoveNodeTo(node.GetId(), matching_node_pos);
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
void NodeMover::MoveNodePinPoses(const core::INode& node, const ImVec2& pos) {
  const auto delta = pos - node.GetPos();

  for (const auto& [pin_id, pin_kind] : core::INode::GetAllPins(node)) {
    const auto pin_pos = pin_poses_.find(pin_id.Get());

    if (pin_pos != pin_poses_.end()) {
      pin_pos->second += delta;
    }
  }
}

///
void NodeMover::MoveTreeTo(const flow::TreeNode& tree_node, const ImVec2& pos) {
  const auto delta = pos - GetTreeRect(tree_node).Min;

  flow::TraverseDepthFirst(
      tree_node,
      [this, &delta](const auto& tree_node) {
        MoveNodeTo(tree_node.node_id, GetNodePos(tree_node.node_id) + delta);
      },
      [](const auto&) {});
}

///
void NodeMover::MarkToMove(ne::NodeId node_id) {
  nodes_to_move_.insert(node_id.Get());
}

///
void NodeMover::MarkNewNodesToMove() {
  const auto& diagram = parent_diagram_->GetDiagram();

  for (const auto& node : diagram.GetNodes()) {
    const auto node_id = node->GetId();

    if (const auto node_is_new = !node_sizes_.contains(node_id.Get())) {
      MarkToMove(node_id);
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