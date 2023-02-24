#ifndef VH_DRAW_WIDGETS_H_
#define VH_DRAW_WIDGETS_H_

#include <imgui_node_editor.h>

#include <optional>
#include <unordered_map>

#include "app_state.h"
#include "draw_main_menu_bar.h"
#include "draw_node_editor.h"

namespace esc {
class EventQueue;

namespace core {
class Project;
class IdGenerator;
}  // namespace core

namespace flow {
class FlowCalculator;
}  // namespace flow

namespace draw {
struct Widgets {
  void Draw(const AppState &app_state);

  MainMenuBar main_menu_bar{};
  NodeEditor node_editor{};
};
}  // namespace draw
}  // namespace esc

#endif  // VH_DRAW_WIDGETS_H_
