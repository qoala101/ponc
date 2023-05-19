/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_main_window.h"

#include "draw_diagram_editor.h"
#include "draw_main_menu_bar.h"
#include "draw_recent_log.h"

namespace vh::ponc::draw {
///
void MainWindow::Draw(coreui::Project& project) {
  diagram_editor_.Draw(project.GetDiagram());
  main_menu_bar_.Draw(project);
  DrawRecentLog(project.GetLog(), main_menu_bar_.GetLogView());
}
}  // namespace vh::ponc::draw