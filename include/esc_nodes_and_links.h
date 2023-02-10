#ifndef VH_ESC_NODES_AND_LINKS_H_
#define VH_ESC_NODES_AND_LINKS_H_

#include <memory>
#include <vector>

#include "core_node.h"
#include "esc_types.h"
#include "imgui_node_editor.h"

class App;

namespace esc {
class NodesAndLinks {
 public:
  NodesAndLinks(std::shared_ptr<App> app,
                std::vector<std::shared_ptr<INodeFactory>>);

  static auto GetSelectedNodeIds() -> std::vector<ne::NodeId>;
  static auto GetSelectedLinkIds() -> std::vector<ne::LinkId>;

  auto GetNodeFactories() -> std::vector<std::shared_ptr<INodeFactory>> &;

  auto GetNodes() -> std::vector<std::shared_ptr<INode>> &;  // return weaks
  auto GetLinks() const -> const std::vector<Link> &;

  auto EmplaceNode(std::shared_ptr<INode> node) -> INode &;

  auto FindNode(ne::NodeId id) -> INode *;
  auto FindPin(ne::PinId id) -> Pin *;

  auto FindLink(ne::LinkId id) -> Link *;
  auto IsPinLinked(ne::PinId id) -> bool;

  void SpawnLink(const Link &link);
  void EraseLinkWithId(ne::LinkId id);
  void EraseNodeWithId(ne::NodeId id);

  void SpawnLinkFromPinToNode(const Pin *pin, const INode *node);

  void LoadFromFile(const std::string &file_path);
  void SaveToFile(const std::string &file_path);
  void DeleteAll();

  void OnFrame();
  void UpdateNodePointerOnPins();
  void UpdatePinValues();
  void ClearAllValuesExceptInput();

 private:
  std::shared_ptr<App> app_{};
  std::vector<std::shared_ptr<INode>> nodes_{};
  std::vector<Link> links_{};

  std::vector<std::shared_ptr<INodeFactory>> node_factories_{};
};
}  // namespace esc

#endif  // VH_ESC_NODES_AND_LINKS_H_
