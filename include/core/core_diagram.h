#ifndef VH_CORE_DIAGRAM_H_
#define VH_CORE_DIAGRAM_H_

#include <memory>
#include <vector>

#include "core_i_node.h"
#include "core_link.h"
#include "cpp_safe_ptr.h"
#include "imgui_node_editor.h"

namespace esc::core {
///
class Diagram {
 public:
  ///
  explicit Diagram(std::vector<std::unique_ptr<INode>> nodes = {},
                   std::vector<Link> links = {});

  ///
  static auto FindNode(const Diagram &diagram, ne::NodeId node_id) -> INode &;
  ///
  static auto FindPinNode(const Diagram &diagram, ne::PinId pin_id)
      -> const INode &;
  ///
  static auto FindLink(Diagram &diagram, ne::LinkId link_id) -> Link &;
  ///
  static auto FindPinLink(const Diagram &diagram, ne::PinId pin_id)
      -> std::optional<const Link *>;
  ///
  static auto FindPinLink(Diagram &diagram, ne::PinId pin_id)
      -> std::optional<Link *>;
  ///
  static auto HasLink(const Diagram &diagram, ne::PinId pin_id) -> bool;

  ///
  auto GetNodes() const -> const std::vector<std::unique_ptr<INode>> &;
  ///
  auto EmplaceNode(std::unique_ptr<INode> node) -> INode &;
  ///
  void DeleteNode(ne::NodeId node_id);
  ///
  auto GetLinks() const -> const std::vector<Link> &;
  ///
  auto EmplaceLink(const Link &link) -> Link &;
  ///
  void DeleteLink(ne::LinkId link_id);

 private:
  ///
  auto GetLinksImpl() -> std::vector<Link> &;

  ///
  std::vector<std::unique_ptr<INode>> nodes_{};
  ///
  std::vector<Link> links_{};
};
}  // namespace esc::core

#endif  // VH_CORE_DIAGRAM_H_
