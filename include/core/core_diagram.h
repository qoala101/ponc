/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_CORE_DIAGRAM_H_
#define VH_CORE_DIAGRAM_H_

#include <memory>
#include <vector>

#include "core_group.h"
#include "core_i_node.h"
#include "core_link.h"
#include "imgui_node_editor.h"

namespace esc::core {
class Diagram {
 public:
  explicit Diagram(std::vector<std::shared_ptr<INode>> nodes = {},
                   std::vector<Link> links = {},
                   std::vector<Group> groups = {});

  auto GetNodes() const -> const std::vector<std::shared_ptr<INode>> &;
  auto EmplaceNode(std::shared_ptr<INode> node)
      -> const std::shared_ptr<INode> &;
  void EraseNode(ne::NodeId node_id);

  auto GetLinks() const -> const std::vector<Link> &;
  auto EmplaceLink(const Link &link) -> Link &;
  void EraseLink(ne::LinkId link_id);

  auto GetGroups() const -> const std::vector<Group> &;
  auto EmplaceGroup(Group group) -> Group &;
  void EraseGroup(const Group &group);

 private:
  std::vector<std::shared_ptr<INode>> nodes_{};
  std::vector<Link> links_{};
  std::vector<Group> groups_{};
};

auto FindPinNode(const Diagram &diagram, ne::PinId pin_id)
    -> const std::shared_ptr<INode> &;

auto FindPinLink(const Diagram &diagram, ne::PinId pin_id)
    -> std::optional<const Link *>;

// // auto FindNode  (const Diagram &diagram, ne::NodeId node_id)
// -> const std::shared_ptr<INode> &;

// auto FindLink(ne::LinkId id) -> Link &;
// auto FindLinkFromPin(ne::PinId pin_id) -> const Link *;

auto GetSelectedNodeIds() -> std::vector<ne::NodeId>;
auto GetSelectedLinkIds() -> std::vector<ne::LinkId>;
}  // namespace esc::core

#endif  // VH_CORE_DIAGRAM_H_
