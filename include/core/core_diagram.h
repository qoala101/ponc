#ifndef VH_CORE_DIAGRAM_H_
#define VH_CORE_DIAGRAM_H_

#include <memory>
#include <vector>

#include "core_node.h"
#include "esc_types.h"
#include "imgui_node_editor.h"

namespace esc::core {
class Diagram {
 public:
  explicit Diagram(std::vector<std::shared_ptr<INodeFactory>>);

  auto GetNodeFactories() const
      -> const std::vector<std::shared_ptr<INodeFactory>> &;
  auto GetNodes() const -> const std::vector<std::shared_ptr<INode>> &;
  auto GetLinks() const -> const std::vector<Link> &;

  auto EmplaceNode(std::shared_ptr<INode> node) -> INode &;
  auto EmplaceLink(const Link &link) -> Link &;

  void EraseNode(ne::NodeId id);
  void EraseLink(ne::LinkId id);

  auto FindNode(ne::NodeId id) -> INode *;
  auto FindPin(ne::PinId id) -> IPin *;
  auto FindLink(ne::LinkId id) -> Link *;
  
  void Clear();

  static auto GetSelectedNodeIds() -> std::vector<ne::NodeId>;
  static auto GetSelectedLinkIds() -> std::vector<ne::LinkId>;




  void LoadFromFile(const std::string &file_path);
  void SaveToFile(const std::string &file_path);

  void OnFrame();
  void UpdateNodePointerOnPins();
  void UpdatePinValues();
  void ClearAllValuesExceptInput();

 private:
  std::vector<std::shared_ptr<INodeFactory>> node_factories_{};
  std::vector<std::shared_ptr<INode>> nodes_{};
  std::vector<Link> links_{};
};
}  // namespace esc::core

#endif  // VH_CORE_DIAGRAM_H_
