#ifndef VH_ESC_NODES_AND_LINKS_H_
#define VH_ESC_NODES_AND_LINKS_H_

#include <vector>

#include "esc_types.h"
#include "imgui_node_editor.h"

class App;

namespace esc {
class NodesAndLinks {
 public:
  explicit NodesAndLinks(std::shared_ptr<App> app);

  static auto GetSelectedNodeIds [[nodiscard]] () -> std::vector<ne::NodeId>;
  static auto GetSelectedLinkIds [[nodiscard]] () -> std::vector<ne::LinkId>;
  static auto GetNodeTypeNames [[nodiscard]] () -> std::vector<std::string>;

  auto SpawnInputNode() -> Node *;
  auto SpawnDividerNode() -> Node *;
  auto Spawn1To2Node() -> Node *;
  auto Spawn1To4Node() -> Node *;
  auto Spawn1To8Node() -> Node *;
  auto Spawn1To16Node() -> Node *;
  auto SpawnClientNode() -> Node *;
  auto SpawnCommentNode() -> Node *;

  auto Spawn1ToNNode(int n) -> Node *;

  auto SpawnNodeByTypeName [[nodiscard]] (const std::string &type_name)
  -> Node *;

  void BuildNodes();

  auto GetNodes [[nodiscard]] () -> std::vector<Node> &;
  auto GetLinks [[nodiscard]] () const -> const std::vector<Link> &;

  auto FindNode(ne::NodeId id) -> Node *;
  auto FindPin(ne::PinId id) -> Pin *;

  auto FindLink(ne::LinkId id) -> Link *;
  auto IsPinLinked(ne::PinId id) -> bool;

  void SpawnLink(const Link &link);
  void EraseLinkWithId(ne::LinkId id);

  void SpawnLinkFromPinToNode(const Pin *pin, const Node *node);

  void LoadFromFile(const std::string &file_path);
  void SafeToFile(const std::string &file_path);
  void DeleteAll();

 private:
  std::shared_ptr<App> app_{};
  std::vector<Node> nodes_{};
  std::vector<Link> links_{};
};
}  // namespace esc

#endif  // VH_ESC_NODES_AND_LINKS_H_
