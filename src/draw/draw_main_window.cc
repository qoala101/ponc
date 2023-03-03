#include "draw_main_window.h"

#include "draw_diagram_editor.h"
#include "draw_main_menu_bar.h"

namespace esc::draw {
///
void MainWindow::Draw(coreui::Project &project) {
  main_menu_bar_.Draw(project);
  diagram_editor_.Draw(project.GetDiagram());
}
}  // namespace esc::draw