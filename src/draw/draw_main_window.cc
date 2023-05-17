#include "draw_main_window.h"

#include "cpp_assert.h"
#include "draw_diagram_editor.h"
#include "draw_main_menu_bar.h"
#include "draw_recent_log.h"
#include "imgui.h"
#include "style_tailwind.h"
#include "style_utils.h"

namespace vh::ponc::draw {
///
void MainWindow::Draw(coreui::Project& project) {
  diagram_editor_.Draw(project.GetDiagram());
  main_menu_bar_.Draw(project);
  DrawRecentLog(project.GetLog(), main_menu_bar_.GetLogView());
}
}  // namespace vh::ponc::draw