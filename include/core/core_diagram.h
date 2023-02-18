/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_CORE_DIAGRAM_H_
#define VH_CORE_DIAGRAM_H_

#include <memory>
#include <vector>

#include "core_free_pin_family.h"
#include "core_group.h"
#include "core_i_family.h"
#include "core_i_node.h"
#include "core_link.h"
#include "core_placeholder_family.h"
#include "imgui_node_editor.h"

namespace esc::core {
// ---
class Diagram {
 public:
  // ---
  explicit Diagram(std::vector<std::shared_ptr<IFamily>> families = {},
                   std::vector<Link> links = {},
                   std::vector<Group> groups = {});

  // ---
  auto GetFamilies [[nodiscard]] () const
      -> const std::vector<std::shared_ptr<IFamily>> &;
  // ---
  auto GetFreePinFamily [[nodiscard]] () const -> FreePinFamily &;
  // ---
  auto GetPlaceholderFamily [[nodiscard]] () const -> PlaceholderFamily &;

  // ---
  auto GetLinks [[nodiscard]] () const -> const std::vector<Link> &;
  // ---
  auto EmplaceLink(const Link &link) -> Link &;
  // ---
  void EraseLink(ne::LinkId link_id);

  // ---
  auto GetGroups [[nodiscard]] () -> const std::vector<Group> &;
  // ---
  auto EmplaceGroup(Group group) -> Group &;
  // ---
  void EraseGroup(const Group &group);

 private:
  // ---
  std::vector<std::shared_ptr<IFamily>> families_{};
  // ---
  std::vector<Link> links_{};
  // ---
  std::vector<Group> groups_{};
  // ---
  std::weak_ptr<FreePinFamily> free_pin_family_{};
  // ---
  std::weak_ptr<PlaceholderFamily> placeholder_family_{};
};

// ---
auto FindNode [[nodiscard]] (const Diagram &diagram, ne::NodeId node_id)
-> const std::shared_ptr<INode> &;
// ---
void EraseNode(Diagram &diagram, ne::NodeId node_id);

// auto FindPinNode(ne::PinId id) -> const std::shared_ptr<INode> &;
// auto FindLink(ne::LinkId id) -> Link &;
// auto FindLinkFromPin(ne::PinId pin_id) -> const Link *;

auto GetSelectedNodeIds() -> std::vector<ne::NodeId>;
auto GetSelectedLinkIds() -> std::vector<ne::LinkId>;
}  // namespace esc::core

#endif  // VH_CORE_DIAGRAM_H_
