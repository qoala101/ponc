/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_DRAW_NODES_H_
#define VH_DRAW_NODES_H_

#include <cstdint>
#include <memory>
#include <unordered_map>

#include "app_state.h"
#include "core_i_node.h"
#include "coreui_i_pin_drawer.h"
#include "draw_texture.h"
#include "flow_node_flow.h"
#include "imgui.h"
#include "imgui_node_editor.h"

namespace esc::draw {
// ---
class Nodes {
 public:
  // ---
  explicit Nodes(const Texture& node_header_texture);

  // ---
  void Draw(const AppState& app_state);
  // ---
  auto GetPinPosition [[nodiscard]] (ne::PinId pin_id) const -> const ImVec2&;

 private:
  // ---
  void DrawNode(const AppState& app_state, core::INode& node);
  // ---
  void DrawPinIconArea(const AppState& app_state,
                       const coreui::IPinDrawer& pin_drawer);

  // ---
  Texture node_header_texture_{};
  // ---
  std::unordered_map<uintptr_t, ImVec2> pin_positions_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_NODES_H_
