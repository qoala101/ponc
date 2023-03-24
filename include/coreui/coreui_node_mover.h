#ifndef VH_PONC_COREUI_NODE_MOVER_H_
#define VH_PONC_COREUI_NODE_MOVER_H_

#include <imgui_node_editor.h>

#include <cstdint>
#include <unordered_set>
#include <vector>

#include "coreui_event_loop.h"
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
  void MoveNode(ne::NodeId node_id);
  ///
  void MoveNodesTo(const std::vector<ne::NodeId> &node_ids, ImVec2 pos);
  ///
  void MovePinTo(ne::PinId pin_id, const ImVec2 &pos);
  ///
  void MakeTree(const flow::TreeNode &tree_node);
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
  void MoveNewNodes();
  ///
  void ApplyMoves() const;

  ///
  cpp::SafePtr<Diagram> parent_diagram_;
  ///
  cpp::SafeOwner safe_owner_{};
  ///
  std::unordered_set<uintptr_t> nodes_to_move_{};
  ///
  std::unordered_map<uintptr_t, ImVec2> node_sizes_{};
  ///
  std::unordered_map<uintptr_t, ImVec2> pin_poses_{};
};
}  // namespace vh::ponc::coreui

#endif  // VH_PONC_COREUI_NODE_MOVER_H_
