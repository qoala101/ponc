/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "coreui_node_mover.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_node_editor.h>

#include <algorithm>
#include <array>
#include <concepts>
#include <functional>
#include <iterator>
#include <map>
#include <memory>
#include <numeric>
#include <unordered_map>
#include <utility>
#include <vector>

#include "core_area.h"
#include "core_diagram.h"
#include "core_i_node.h"
#include "core_id_value.h"
#include "core_link.h"
#include "coreui_diagram.h"
#include "coreui_i_node_traits.h"  // IWYU pragma: keep
#include "cpp_assert.h"
#include "flow_tree_node.h"
#include "flow_tree_traversal.h"

namespace vh::ponc::coreui {
namespace {
///
void TraverseOtherNodes(
    const std::vector<flow::TreeNode>& tree_nodes,
    const std::vector<flow::TreeNode>& skip_trees,
    const std::invocable<const flow::TreeNode&> auto& visitor) {
  for (const auto& tree_node : tree_nodes) {
    auto skip_node_id = std::optional<ne::NodeId>{};

    flow::TraverseDepthFirst(
        tree_node,
        [&skip_trees, &visitor, &skip_node_id](const auto& tree_node) {
          if (skip_node_id.has_value()) {
            return;
          }

          if (std::any_of(skip_trees.cbegin(), skip_trees.cend(),
                          [&tree_node](const auto& skip_tree) {
                            return skip_tree.node_id == tree_node.node_id;
                          })) {
            skip_node_id = tree_node.node_id;
            return;
          }

          visitor(tree_node);
        },
        [&skip_node_id](const auto& tree_node) {
          if (skip_node_id.has_value() &&
              (tree_node.node_id == *skip_node_id)) {
            skip_node_id.reset();
          }
        });
  }
}

///
auto GetParentTrees(const std::vector<flow::TreeNode>& tree_nodes,
                    const std::vector<flow::TreeNode>& child_trees) {
  if (child_trees.empty()) {
    return std::vector<flow::TreeNode>{};
  }

  auto parent_trees = std::vector<flow::TreeNode>{};

  TraverseOtherNodes(
      tree_nodes, child_trees,
      [&child_trees, &parent_trees](const auto& tree_node) {
        auto parent_tree = flow::TreeNode{tree_node.node_id};

        for (const auto& child_node : tree_node.child_nodes) {
          if (std::any_of(child_trees.cbegin(), child_trees.cend(),
                          [&child_node](const auto& child_tree) {
                            return child_tree.node_id ==
                                   child_node.second.node_id;
                          })) {
            parent_tree.child_nodes.emplace(child_node);
          }
        }

        if (!parent_tree.child_nodes.empty()) {
          parent_trees.emplace_back(std::move(parent_tree));
        }
      });

  return parent_trees;
}
}  // namespace

///
NodeMover::NodeMover(cpp::SafePtr<Diagram> parent_diagram,
                     cpp::SafePtr<core::Settings> settings)
    : parent_diagram_{std::move(parent_diagram)},
      settings_{std::move(settings)} {}

///
void NodeMover::OnFrame() {
  MarkNewItemsToMove();
  ApplyMoves();

  nodes_to_move_.clear();
  areas_to_move_.clear();
  item_sizes_.clear();
  pin_poses_.clear();
}

///
void NodeMover::MoveNodeTo(ne::NodeId node_id, const ImVec2& pos) {
  const auto& diagram = parent_diagram_->GetDiagram();
  auto& node = core::Diagram::FindNode(diagram, node_id);

  MoveNodePinPoses(node, pos);
  node.SetPos(pos);
  MarkNodeToMove(node_id);
}

///
auto NodeMover::DoNodesNeedSpacing(ne::NodeId first_node,
                                   ne::NodeId second_node) const {
  const auto node_ids = std::array{first_node, second_node};
  auto& diagram = parent_diagram_->GetDiagram();

  return std::any_of(
      node_ids.cbegin(), node_ids.cend(), [&diagram](const auto node_id) {
        auto& node = core::Diagram::FindNode(diagram, node_id);
        const auto node_has_header =
            node.CreateUiTraits()->CreateHeaderTraits().has_value();
        return node_has_header;
      });
}

///
void NodeMover::ArrangeVerticallyAt(const std::vector<ne::NodeId>& node_ids,
                                    const ImVec2& pos) {
  if (node_ids.empty()) {
    return;
  }

  auto next_node_pos = pos;

  for (auto node_id = node_ids.cbegin(); node_id != node_ids.cend();
       ++node_id) {
    MoveNodeTo(*node_id, next_node_pos);
    next_node_pos.y += GetNodeSize(*node_id).y;

    const auto next_id = std::next(node_id);

    if ((next_id != node_ids.cend()) &&
        DoNodesNeedSpacing(*node_id, *next_id)) {
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
auto NodeMover::GetTakenPinsRect(
    const std::vector<flow::TreeNode>& tree_nodes) const {
  auto rect = std::optional<ImRect>{};

  for (const auto& tree_node : tree_nodes) {
    for (const auto& [pin_id, child_node] : tree_node.child_nodes) {
      const auto pin_pos = GetPinPos(pin_id);

      if (rect.has_value()) {
        rect->Add(pin_pos);
        continue;
      }

      rect.emplace(pin_pos, pin_pos);
    }
  }

  return rect;
}

///
auto NodeMover::CalculateArrangedChildrenY(
    const std::vector<flow::TreeNode>& parent_trees) const {
  Expects(!parent_trees.empty());

  const auto& [first_output_pin, first_child] =
      *parent_trees.front().child_nodes.cbegin();
  const auto tree_top_to_first_input_pin_distance =
      GetOtherPinPos(first_output_pin).y - GetTreeRect(first_child).Min.y;

  const auto [last_output_pin, last_child] =
      *parent_trees.back().child_nodes.crbegin();
  const auto last_input_pin_to_tree_bot_distance =
      GetTreeRect(last_child).Max.y - GetOtherPinPos(last_output_pin).y;

  const auto padding = tree_top_to_first_input_pin_distance +
                       last_input_pin_to_tree_bot_distance;
  const auto spacing = static_cast<float>(settings_->arrange_vertical_spacing) *
                       static_cast<float>(parent_trees.size() - 1);

  const auto child_pins_height = std::accumulate(
      parent_trees.cbegin(), parent_trees.cend(), spacing - padding,
      [this](const auto height, const auto& output_tree_parent) {
        Expects(!output_tree_parent.child_nodes.empty());

        const auto& [first_pin, first_child] =
            *output_tree_parent.child_nodes.cbegin();
        const auto [last_pin, last_child] =
            *output_tree_parent.child_nodes.crbegin();

        return height + GetTreeRect(last_child).Max.y -
               GetTreeRect(first_child).Min.y;
      });

  const auto parent_pins_rect = GetTakenPinsRect(parent_trees);
  Expects(parent_pins_rect.has_value());

  return parent_pins_rect->GetCenter().y - child_pins_height / 2 -
         tree_top_to_first_input_pin_distance;
}

///
void NodeMover::ArrangeAsTreeVisitNode(const flow::TreeNode& tree_node) {
  const auto& child_nodes = tree_node.child_nodes;

  if (child_nodes.empty()) {
    return;
  }

  auto next_child_y = 0.F;

  for (auto child_node = child_nodes.cbegin(); child_node != child_nodes.cend();
       ++child_node) {
    MoveTreeTo(child_node->second, {0, next_child_y});

    const auto child_tree_rect = GetTreeRect(child_node->second);
    next_child_y += child_tree_rect.GetHeight();

    if (DoesChildNeedSpacing(tree_node, child_node)) {
      next_child_y += static_cast<float>(settings_->arrange_vertical_spacing);
    }
  }

  const auto children_x =
      GetNodeRect(tree_node.node_id).Max.x +
      static_cast<float>(settings_->arrange_horizontal_spacing);
  const auto children_y = CalculateArrangedChildrenY({tree_node});

  MoveChildTreesTo(tree_node, ImVec2{children_x, children_y});
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
  if (tree_nodes.empty()) {
    return;
  }

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

///
void NodeMover::ArrangeChildrenAsTrees(
    const std::vector<flow::TreeNode>& tree_nodes) {
  auto parent_node_poses =
      std::unordered_map<core::IdValue<ne::NodeId>, ImVec2>{};
  parent_node_poses.reserve(tree_nodes.size());

  std::transform(tree_nodes.cbegin(), tree_nodes.cend(),
                 std::inserter(parent_node_poses, parent_node_poses.begin()),
                 [this](const auto& tree_node) {
                   return std::pair{tree_node.node_id.Get(),
                                    GetNodePos(tree_node.node_id)};
                 });

  ArrangeAsTrees(tree_nodes);

  for (const auto& [node_id, node_pos] : parent_node_poses) {
    MoveNodeTo(node_id, node_pos);
  }
}

///
auto NodeMover::TakenPinPosLess(const flow::TreeNode& left,
                                const flow::TreeNode& right) const {
  Expects(!left.child_nodes.empty());
  const auto first_left_pin = left.child_nodes.cbegin()->first;

  Expects(!right.child_nodes.empty());
  const auto first_right_pin = right.child_nodes.cbegin()->first;

  return GetPinPos(first_left_pin).y < GetPinPos(first_right_pin).y;
}

///
void NodeMover::ArrangeAsNewTrees(
    const std::vector<flow::TreeNode>& tree_nodes) {
  if (tree_nodes.empty()) {
    return;
  }

  const auto& flow_trees = parent_diagram_->GetFlowTrees();

  auto parent_trees = GetParentTrees(flow_trees, tree_nodes);
  std::stable_sort(parent_trees.begin(), parent_trees.end(),
                   std::bind_front(&NodeMover::TakenPinPosLess, this));

  Expects(!flow_trees.empty());
  auto other_nodes_rect = GetNodeRect(flow_trees.front().node_id);
  TraverseOtherNodes(flow_trees, tree_nodes,
                     [this, &other_nodes_rect](const auto& tree_node) {
                       other_nodes_rect.Add(GetNodeRect(tree_node.node_id));
                     });

  const auto next_child_x =
      other_nodes_rect.Max.x +
      static_cast<float>(settings_->arrange_horizontal_spacing);

  ArrangeChildrenAsTrees(parent_trees);
  auto next_child_y = CalculateArrangedChildrenY(parent_trees);

  for (const auto& output_tree_parent : parent_trees) {
    MoveChildTreesTo(output_tree_parent, ImVec2{next_child_x, next_child_y});

    Expects(!output_tree_parent.child_nodes.empty());
    const auto last_child = output_tree_parent.child_nodes.crbegin();

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
  Expects(item_sizes_.contains(node_id_value));
  return item_sizes_.at(node_id_value);
}

///
void NodeMover::SetItemSize(ne::NodeId node_id, const ImVec2& size) {
  item_sizes_.emplace(node_id, size);
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

    if (pin_pos != pin_poses_.cend()) {
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
void NodeMover::MoveChildTreesTo(const flow::TreeNode& tree_node,
                                 const ImVec2& pos) {
  if (tree_node.child_nodes.empty()) {
    return;
  }

  const auto first_child = tree_node.child_nodes.cbegin();
  const auto delta = pos - GetTreeRect(first_child->second).Min;

  for (const auto& [pin_id, output_tree] : tree_node.child_nodes) {
    MoveTreeTo(output_tree, GetTreeRect(output_tree).Min + delta);
  }
}

///
void NodeMover::MarkNodeToMove(ne::NodeId node_id) {
  nodes_to_move_.insert(node_id.Get());
}

///
void NodeMover::MarkAreaToMove(core::AreaId area_id) {
  areas_to_move_.insert(area_id.Get());
}

///
void NodeMover::MarkNewItemsToMove() {
  const auto& diagram = parent_diagram_->GetDiagram();

  for (const auto& node : diagram.GetNodes()) {
    const auto node_id = node->GetId();

    if (const auto node_is_new = !item_sizes_.contains(node_id.Get())) {
      MarkNodeToMove(node_id);
    }
  }

  for (const auto& area : diagram.GetAreas()) {
    if (const auto area_is_new = !item_sizes_.contains(area.id.Get())) {
      MarkAreaToMove(area.id);
    }
  }
}

///
void NodeMover::ApplyMoves() const {
  const auto& diagram = parent_diagram_->GetDiagram();

  for (const auto node_id : nodes_to_move_) {
    const auto& node = core::Diagram::FindNode(diagram, node_id);
    ne::SetNodePosition(node_id, node.GetPos());
  }

  for (const auto area_id : areas_to_move_) {
    const auto& area = core::Diagram::FindArea(diagram, area_id);
    ne::SetNodePosition(area_id, area.pos);
    ne::SetGroupSize(area_id, area.size);
  }
}
}  // namespace vh::ponc::coreui