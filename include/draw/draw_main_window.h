#ifndef VH_PONC_DRAW_MAIN_WINDOW_H_
#define VH_PONC_DRAW_MAIN_WINDOW_H_

#include <imgui_node_editor.h>

#include <optional>
#include <unordered_map>

#include "coreui_project.h"
#include "draw_diagram_editor.h"
#include "draw_main_menu_bar.h"

namespace vh::ponc::draw {
///
class MainWindow {
 public:
  ///
  void Draw(coreui::Project &project);

 private:
  ///
  DiagramEditor diagram_editor_{};
  ///
  MainMenuBar main_menu_bar_{};
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_MAIN_WINDOW_H_
