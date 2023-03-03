#ifndef VH_DRAW_MAIN_WINDOW_H_
#define VH_DRAW_MAIN_WINDOW_H_

#include <imgui_node_editor.h>

#include <optional>
#include <unordered_map>

#include "coreui_project.h"
#include "draw_diagram_editor.h"
#include "draw_main_menu_bar.h"

namespace esc::draw {
///
class MainWindow {
 public:
  ///
  void Draw(coreui::Project &project);

 private:
  ///
  MainMenuBar main_menu_bar_{};
  ///
  DiagramEditor diagram_editor_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_MAIN_WINDOW_H_
