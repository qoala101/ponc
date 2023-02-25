#ifndef VH_DRAW_WIDGETS_H_
#define VH_DRAW_WIDGETS_H_

#include <imgui_node_editor.h>

#include <optional>
#include <unordered_map>

#include "draw_main_menu_bar.h"
#include "draw_node_editor.h"
#include "frame_node.h"

namespace esc::draw {
class MainWindow {
 public:
  void Draw(frame::Frame &frame);

 private:
  MainMenuBar main_menu_bar{};
  NodeEditor node_editor{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_WIDGETS_H_
