#ifndef VH_CORE_DIAGRAM_H_
#define VH_CORE_DIAGRAM_H_

#include <memory>
#include <vector>

#include "core_i_family.h"
#include "core_i_node.h"
#include "core_link.h"
#include "draw_i_pin_drawer.h"
#include "imgui_node_editor.h"

namespace esc::core {
class Diagram {
 public:
  explicit Diagram(std::vector<std::shared_ptr<IFamily>> families = {},
                   std::vector<Link> links = {});

  Diagram(const Diagram &) = delete;
  Diagram(Diagram &&) noexcept = default;

  auto operator=(const Diagram &) noexcept -> Diagram & = delete;
  auto operator=(Diagram &&) noexcept -> Diagram & = default;

  ~Diagram();

  auto GetFamilies() const -> const std::vector<std::shared_ptr<IFamily>> &;
  auto GetLinks() const -> const std::vector<Link> &;

  auto EmplaceLink(const Link &link) -> Link &;

  void EraseNode(ne::NodeId id);
  void EraseLink(ne::LinkId id);

  auto FindNode(ne::NodeId id) -> INode &;
  auto FindPin(ne::PinId id, const State &state) -> std::unique_ptr<draw::IPinDrawer>;
  auto FindLink(ne::LinkId id) -> Link &;

  static auto GetSelectedNodeIds() -> std::vector<ne::NodeId>;
  static auto GetSelectedLinkIds() -> std::vector<ne::LinkId>;

  // void OnFrame();
  // void UpdatePinValues();
  // void ClearAllValuesExceptInput();

 private:
  std::vector<std::shared_ptr<IFamily>> families_{};
  std::vector<Link> links_{};
};
}  // namespace esc::core

#endif  // VH_CORE_DIAGRAM_H_
