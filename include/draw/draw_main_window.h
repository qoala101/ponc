/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_MAIN_WINDOW_H_
#define VH_PONC_DRAW_MAIN_WINDOW_H_

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
