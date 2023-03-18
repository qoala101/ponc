#ifndef VH_DRAW_MAIN_MENU_BAR_H_
#define VH_DRAW_MAIN_MENU_BAR_H_

#include "core_settings.h"
#include "coreui_diagram.h"
#include "coreui_project.h"
#include "draw_flow_tree_view.h"
#include "draw_new_project_dialog.h"
#include "draw_nodes_view.h"
#include "draw_open_file_dialog.h"
#include "draw_save_as_file_dialog.h"
#include "draw_settings_view.h"

namespace esc::draw {
///
class MainMenuBar {
 public:
  ///
  void Draw(coreui::Project &project);

 private:
  ///
  void DrawFileMenu(coreui::Project &project);
  ///
  void DrawViewMenu();
  ///
  void DrawDialogs(coreui::Project &project);
  ///
  void DrawViews(const coreui::Diagram &diagram, core::Settings &settings);

  ///
  NewProjectDialog new_project_dialog_{};
  ///
  OpenFileDialog open_file_dialog_{};
  ///
  SaveAsFileDialog save_as_file_dialog_{};
  ///
  NodesView nodes_view_{};
  ///
  FlowTreeView flow_tree_view_{};
  ///
  SettingsView settings_view_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_MAIN_MENU_BAR_H_
