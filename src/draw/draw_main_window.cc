/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_main_window.h"

#include <string>

#include "draw_diagram_editor.h"
#include "draw_main_menu_bar.h"
#include "draw_recent_log.h"

namespace vh::ponc::draw {
///
MainWindow::MainWindow()
    : exit_dialog_{{.title = "Exit",
                    .question = "Current project would not be saved.\nProceed?",
                    .accept = "Exit"}} {}

///
void MainWindow::Draw(const Callbacks& callbacks, coreui::Project& project) {
  ImGui::DockSpaceOverViewport();

  diagram_editor_.Draw(project.GetDiagram(), project.GetProject());
  main_menu_bar_.Draw(project);
  exit_dialog_.Draw({.accepted = callbacks.exit_confirmed});
  DrawRecentLog(project.GetLog(), main_menu_bar_.GetLogView());
}

///
void MainWindow::OpenExitDialog() { exit_dialog_.Open(); }

///
void MainWindow::SaveState(Globals& globals) const {
  main_menu_bar_.SaveState(globals);
}

///
void MainWindow::RestoreState(const Globals& globals) {
  main_menu_bar_.RestoreState(globals);
}
}  // namespace vh::ponc::draw