#ifndef VH_PONC_COREUI_NODE_MOVER_H_
#define VH_PONC_COREUI_NODE_MOVER_H_

#include <imgui_node_editor.h>

#include <cstdint>
#include <unordered_set>
#include <vector>

#include "core_id_value.h"
#include "coreui_linker.h"
#include "cpp_safe_ptr.h"
#include "flow_tree.h"
#include "imgui.h"

namespace vh::ponc::coreui {
///
class NodeMover {
 public:
  ///
  explicit NodeMover(cpp::SafePtr<Diagram> parent_diagram);

  ///
  void OnFrame();
  ///
  void MoveNodeTo(ne::NodeId node_id, const ImVec2 &pos);
  ///
  void MoveNodesTo(const std::vector<ne::NodeId> &node_ids, const ImVec2 &pos);
  ///
  void MovePinTo(ne::PinId pin_id, const ImVec2 &pos);
  ///
  void MakeTree(const flow::TreeNode &tree_node);
  ///
  void MakeTrees(const std::vector<flow::TreeNode> &tree_nodes);
  ///
  auto GetNodeSize(ne::NodeId node_id) const -> const ImVec2 &;
  ///
  void SetNodeSize(ne::NodeId node_id, const ImVec2 &size);
  ///
  auto GetPinPos(ne::PinId pin_id) const -> const ImVec2 &;
  ///
  void SetPinPos(ne::PinId pin_id, const ImVec2 &pos);

 private:
  ///
  void MoveTreeTo(const flow::TreeNode &tree_node, const ImVec2 &pos);
  ///
  void MarkToMove(ne::NodeId node_id);
  ///
  void MarkNewNodesToMove();
  ///
  void ApplyMoves() const;
  ///
  auto GetNodePos(ne::NodeId node_id);
  ///
  auto GetNodeRect(ne::NodeId node_id);
  ///
  auto GetTreeRect(const flow::TreeNode &tree_node);
  ///
  void MakeTreeImpl(const flow::TreeNode &tree_node);

  ///
  cpp::SafePtr<Diagram> parent_diagram_;
  ///
  cpp::SafeOwner safe_owner_{};
  ///
  std::unordered_set<core::IdValue<ne::NodeId>> nodes_to_move_{};
  ///
  std::unordered_map<core::IdValue<ne::NodeId>, ImVec2> node_sizes_{};
  ///
  std::unordered_map<core::IdValue<ne::PinId>, ImVec2> pin_poses_{};
};
}  // namespace vh::ponc::coreui

#endif  // VH_PONC_COREUI_NODE_MOVER_H_
