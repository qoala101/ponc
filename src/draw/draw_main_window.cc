#include "draw_main_window.h"

#include "draw_diagram.h"
#include "draw_main_menu_bar.h"

namespace esc::draw {
void MainWindow::Draw(coreui::Project &frame) {
  main_menu_bar.Draw(frame);
  node_editor.Draw(frame);
}
}  // namespace esc::draw