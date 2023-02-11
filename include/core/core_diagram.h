#ifndef VH_CORE_DIAGRAM_H_
#define VH_CORE_DIAGRAM_H_

#include <memory>
#include <vector>

#include "core_i_node.h"
#include "core_i_node_factory.h"
#include "core_link.h"
#include "imgui_node_editor.h"

namespace esc::core {
class Diagram {
 public:
  Diagram(std::vector<std::shared_ptr<INodeFactory>> node_factories,
          std::vector<std::shared_ptr<INode>> nodes, std::vector<Link> links);

  Diagram(const Diagram &) = delete;
  Diagram(Diagram &&) noexcept = default;

  auto operator=(const Diagram &) noexcept -> Diagram & = delete;
  auto operator=(Diagram &&) noexcept -> Diagram & = default;

  ~Diagram();

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

  static auto GetSelectedNodeIds() -> std::vector<ne::NodeId>;
  static auto GetSelectedLinkIds() -> std::vector<ne::LinkId>;

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
