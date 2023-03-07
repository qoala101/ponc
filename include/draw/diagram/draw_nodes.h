#ifndef VH_DRAW_NODES_H_
#define VH_DRAW_NODES_H_

#include <cstdint>
#include <memory>
#include <unordered_map>

#include "coreui_node.h"
#include "imgui.h"

namespace esc::draw {
class Nodes {
 public:
  void Draw(const std::vector<coreui::Node> &nodes);

  ///
  auto GetDrawnPinTipPos(ne::PinId pin_id) const -> ImVec2;

 private:
  void DrawNode(const coreui::Node &node);

  ///
  std::unordered_map<uintptr_t, ImVec2> drawn_pin_tip_poses_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_NODES_H_
