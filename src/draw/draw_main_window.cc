#include "draw_main_window.h"

#include "draw_diagram_editor.h"
#include "draw_main_menu_bar.h"

namespace vh::ponc::draw {
///
void MainWindow::Draw(coreui::Project &project) {
  diagram_editor_.Draw(project.GetDiagram());
  main_menu_bar_.Draw(project);
}
}  // namespace vh::ponc::draw