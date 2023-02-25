#include "draw_main_window.h"

#include "draw_main_menu_bar.h"
#include "draw_node_editor.h"

namespace esc::draw {
void MainWindow::Draw(frame::Frame &frame) {
  main_menu_bar.Draw(frame);
  node_editor.Draw(frame);
}
}  // namespace esc::draw