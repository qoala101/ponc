#ifndef VH_DRAW_NODES_H_
#define VH_DRAW_NODES_H_

#include <cstdint>
#include <memory>
#include <unordered_map>

#include "core_i_node.h"
#include "core_settings.h"
#include "coreui_i_pin_traits.h"
#include "draw_creation.h"
#include "draw_settings_view.h"
#include "draw_texture.h"
#include "flow_node_flow.h"
#include "frame_node.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_node_editor.h"

namespace esc::frame {
struct Node;
struct Pin;
}  // namespace esc::frame

namespace esc::draw {
class Nodes {
 public:
  void Draw(coreui::Frame& frame);

 private:
  void DrawNode_v2(coreui::Frame& frame, const coreui::Node& node);
  void DrawPinIconArea_v2(coreui::Frame& frame, const coreui::Pin& pin);
};
}  // namespace esc::draw

#endif  // VH_DRAW_NODES_H_
