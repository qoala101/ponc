#include "draw_main_window.h"

namespace esc::draw {
MainWindow::MainWindow(std::shared_ptr<AppState> app_state)
    : menu_bar_{std::move(app_state)} {}

void MainWindow::Draw() { menu_bar_.Draw(); }
}  // namespace esc::draw