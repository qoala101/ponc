/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_MAIN_MENU_BAR_H_
#define VH_PONC_DRAW_MAIN_MENU_BAR_H_

#include "coreui_project.h"
#include "draw_calculator_view.h"
#include "draw_diagrams_view.h"
#include "draw_flow_tree_view.h"
#include "draw_log_view.h"
#include "draw_node_view.h"
#include "draw_nodes_view.h"
#include "draw_open_file_dialog.h"
#include "draw_question_dialog.h"
#include "draw_save_as_file_dialog.h"
#include "draw_settings_view.h"

namespace vh::ponc::draw {
///
class MainMenuBar {
 public:
  ///
  MainMenuBar();

  ///
  void Draw(coreui::Project &project);
  ///
  auto GetLogView() const -> const LogView &;

 private:
  ///
  void DrawFileMenu(coreui::Project &project);
  ///
  void DrawViewMenu();
  ///
  void DrawDialogs(coreui::Project &project);
  ///
  void DrawViews(coreui::Project &project);

  ///
  QuestionDialog new_project_dialog_;
  ///
  OpenFileDialog open_file_dialog_{};
  ///
  SaveAsFileDialog save_as_file_dialog_{};
  ///
  NodeView node_view_{};
  ///
  NodesView nodes_view_{};
  ///
  DiagramsView diagrams_view_{};
  ///
  FlowTreeView flow_tree_view_{};
  ///
  CalculatorView calculator_view_{};
  ///
  LogView log_view_{};
  ///
  SettingsView settings_view_{};
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_MAIN_MENU_BAR_H_
