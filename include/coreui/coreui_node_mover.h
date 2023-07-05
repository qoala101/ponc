/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_COREUI_NODE_MOVER_H_
#define VH_PONC_COREUI_NODE_MOVER_H_

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_node_editor.h>

#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "core_area.h"
#include "core_i_node.h"
#include "core_id_value.h"
#include "core_settings.h"
#include "coreui_linker.h"
#include "cpp_safe_ptr.h"
#include "flow_tree_node.h"

namespace vh::ponc::coreui {
///
class NodeMover {
 public:
  ///
  NodeMover(cpp::SafePtr<Diagram> parent_diagram,
            cpp::SafePtr<core::Settings> settings);

  ///
  void OnFrame();
  ///
  void MoveNodeTo(ne::NodeId node_id, const ImVec2 &pos);
  ///
  void ArrangeVerticallyAt(const std::vector<ne::NodeId> &node_ids,
                           const ImVec2 &pos);
  ///
  void MovePinTo(ne::PinId pin_id, const ImVec2 &pos);
  ///
  void ArrangeAsTree(const flow::TreeNode &tree_node);
  ///
  void ArrangeAsTrees(const std::vector<flow::TreeNode> &tree_nodes);
  ///
  void ArrangeAsNewTrees(const std::vector<flow::TreeNode> &tree_nodes);
  ///
  auto GetNodeSize(ne::NodeId node_id) const -> const ImVec2 &;
  ///
  void SetItemSize(ne::NodeId node_id, const ImVec2 &size);
  ///
  auto GetPinPos(ne::PinId pin_id) const -> const ImVec2 &;
  ///
  void SetPinPos(ne::PinId pin_id, const ImVec2 &pos);

 private:
  ///
  void MoveNodePinPoses(const core::INode &node, const ImVec2 &pos);
  ///
  void MoveTreeTo(const flow::TreeNode &tree_node, const ImVec2 &pos);
  ///
  void MoveChildTreesTo(const flow::TreeNode &tree_node, const ImVec2 &pos);
  ///
  void MarkNodeToMove(ne::NodeId node_id);
  ///
  void MarkAreaToMove(core::AreaId area_id);
  ///
  void MarkNewItemsToMove();
  ///
  void ApplyMoves() const;
  ///
  auto GetNodePos(ne::NodeId node_id) const;
  ///
  auto GetNodeRect(ne::NodeId node_id) const;
  ///
  auto GetTreeRect(const flow::TreeNode &tree_node) const;
  ///
  auto GetOtherPinPos(ne::PinId pin_id) const -> const ImVec2 &;
  ///
  auto DoNodesNeedSpacing(ne::NodeId first_node, ne::NodeId second_node) const;
  ///
  auto DoesChildNeedSpacing(
      const flow::TreeNode &tree_node,
      decltype(flow::TreeNode::child_nodes)::const_iterator child_node) const;
  ///
  auto GetTakenPinsRect(const std::vector<flow::TreeNode> &tree_nodes) const;
  ///
  auto CalculateArrangedChildrenY(
      const std::vector<flow::TreeNode> &parent_trees) const;
  ///
  void ArrangeAsTreeVisitNode(const flow::TreeNode &tree_node);
  ///
  void ArrangeAsTreeImpl(const flow::TreeNode &tree_node);
  ///
  void ArrangeChildrenAsTrees(const std::vector<flow::TreeNode> &tree_nodes);
  ///
  auto TakenPinPosLess(const flow::TreeNode &left,
                       const flow::TreeNode &right) const;

  ///
  cpp::SafePtr<Diagram> parent_diagram_;
  ///
  cpp::SafePtr<core::Settings> settings_;
  ///
  cpp::SafeOwner safe_owner_{};
  ///
  std::unordered_set<core::IdValue<ne::NodeId>> nodes_to_move_{};
  ///
  std::unordered_set<core::IdValue<core::AreaId>> areas_to_move_{};
  ///
  // TODO(vh): Those are never cleared.
  std::unordered_map<core::IdValue<ne::NodeId>, ImVec2> item_sizes_{};
  ///
  std::unordered_map<core::IdValue<ne::PinId>, ImVec2> pin_poses_{};
};
}  // namespace vh::ponc::coreui

#endif  // VH_PONC_COREUI_NODE_MOVER_H_
