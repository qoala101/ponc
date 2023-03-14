#ifndef VH_DRAW_MAIN_MENU_BAR_H_
#define VH_DRAW_MAIN_MENU_BAR_H_

#include "coreui_project.h"
#include "draw_new_project_dialog.h"
#include "draw_open_file_dialog.h"
#include "draw_save_as_file_dialog.h"

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
  void DrawDialogs(coreui::Project &project);

  ///
  NewProjectDialog new_project_dialog_{};
  ///
  OpenFileDialog open_file_dialog_{};
  ///
  SaveAsFileDialog save_as_file_dialog_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_MAIN_MENU_BAR_H_
