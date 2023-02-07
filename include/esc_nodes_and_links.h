#ifndef VH_ESC_NODES_HANDLE_H_
#define VH_ESC_NODES_HANDLE_H_

#include <vector>

#include "esc_auto_incrementable.h"
#include "esc_types.h"
#include "imgui_node_editor.h"

namespace esc {
class NodesAndLinks {
 public:
  explicit NodesAndLinks(std::shared_ptr<AutoIncrementable> auto_object_id);

  auto SpawnInputActionNode() -> Node *;
  auto SpawnBranchNode() -> Node *;
  auto SpawnDoNNode() -> Node *;
  auto SpawnOutputActionNode() -> Node *;
  auto SpawnPrintStringNode() -> Node *;
  auto SpawnMessageNode() -> Node *;
  auto SpawnSetTimerNode() -> Node *;
  auto SpawnLessNode() -> Node *;
  auto SpawnWeirdNode() -> Node *;
  auto SpawnTraceByChannelNode() -> Node *;
  auto SpawnTreeSequenceNode() -> Node *;
  auto SpawnTreeTaskNode() -> Node *;
  auto SpawnTreeTask2Node() -> Node *;
  auto SpawnComment() -> Node *;
  auto SpawnHoudiniTransformNode() -> Node *;
  auto SpawnHoudiniGroupNode() -> Node *;

  void BuildNodes();

  auto GetNodes [[nodiscard]] () -> std::vector<Node> &;
  auto GetLinks [[nodiscard]] () const -> const std::vector<Link> &;

  auto FindNode(ne::NodeId id) -> Node *;
  auto FindPin(ne::PinId id) -> Pin *;

  auto FindLink(ne::LinkId id) -> Link *;
  auto IsPinLinked(ne::PinId id) -> bool;

  void SpawnLink(const Link &link);
  void EraseLinkWithId(ne::LinkId id);

 private:
  std::shared_ptr<esc::AutoIncrementable> auto_object_id_{};
  std::vector<Node> nodes_{};
  std::vector<Link> links_{};
};
}  // namespace esc

#endif  // VH_ESC_NODES_HANDLE_H_
