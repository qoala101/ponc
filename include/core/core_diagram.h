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
#include "coreui_i_pin_drawer.h"
#include "imgui_node_editor.h"

namespace esc::core {
class Diagram {
 public:
  explicit Diagram(std::vector<std::shared_ptr<IFamily>> families = {},
                   std::vector<Link> links = {});

  auto GetFamilies() const -> const std::vector<std::shared_ptr<IFamily>> &;
  auto GetLinks() const -> const std::vector<Link> &;
  auto GetGroups() -> std::vector<Group> &;

  auto GetPlaceholderFamily() const -> PlaceholderFamily &;
  auto GetFreePinFamily() const -> FreePinFamily &;

  auto EmplaceLink(const Link &link) -> Link &;
  auto EmplaceGroup(std::vector<ne::NodeId> node_ids) -> Group &;

  void EraseNode(ne::NodeId id);
  void EraseLink(ne::LinkId id);
  void EraseGroup(Group *group);

  auto FindNode(ne::NodeId id) -> INode &;
  auto FindNodePTR(ne::NodeId id) -> const std::shared_ptr<INode> &;
  auto FindPin(ne::PinId id, const State &state)
      -> std::unique_ptr<coreui::IPinDrawer>;
  auto FindPinNode(ne::PinId id) -> const std::shared_ptr<INode> &;
  auto FindLink(ne::LinkId id) -> Link &;
  auto FindLinkFromPin(ne::PinId pin_id) -> const Link *;

  static auto GetSelectedNodeIds() -> std::vector<ne::NodeId>;
  static auto GetSelectedLinkIds() -> std::vector<ne::LinkId>;

  // void OnFrame();
  // void UpdatePinValues();
  // void ClearAllValuesExceptInput();

 private:
  std::vector<std::shared_ptr<IFamily>> families_{};
  std::vector<Link> links_{};
  std::vector<Group> groups_{};
  std::weak_ptr<PlaceholderFamily> placeholder_family_{};
  std::weak_ptr<FreePinFamily> free_pin_family_{};
};
}  // namespace esc::core

#endif  // VH_CORE_DIAGRAM_H_
