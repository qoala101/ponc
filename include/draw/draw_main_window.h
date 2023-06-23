/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_MAIN_WINDOW_H_
#define VH_PONC_DRAW_MAIN_WINDOW_H_

#include "coreui_project.h"
#include "cpp_callbacks.h"
#include "draw_diagram_editor.h"
#include "draw_main_menu_bar.h"
#include "draw_question_dialog.h"

namespace vh::ponc::draw {
///
class MainWindow {
 public:
  ///
  struct Callbacks {
    ///
    cpp::Signal<> exit_confirmed{};
  };

  ///
  MainWindow();

  ///
  void Draw(const Callbacks &callbacks, coreui::Project &project);
  ///
  void OpenExitDialog();

 private:
  ///
  DiagramEditor diagram_editor_{};
  ///
  MainMenuBar main_menu_bar_{};
  ///
  QuestionDialog exit_dialog_;
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_MAIN_WINDOW_H_
