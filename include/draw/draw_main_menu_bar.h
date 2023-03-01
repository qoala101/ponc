#ifndef VH_DRAW_MAIN_MENU_BAR_H_
#define VH_DRAW_MAIN_MENU_BAR_H_

#include "draw_open_file_dialog.h"
#include "draw_save_as_file_dialog.h"
#include "draw_settings_view.h"
#include "coreui_project.h"

namespace esc::draw {
class MainMenuBar {
 public:
  void Draw(coreui::Project &frame);

 private:
  static void SlotOpenFileSelected(coreui::Project &frame,
                                   std::string file_path);
  static void SlotSaveAsFileSelected(coreui::Project &frame,
                                     std::string file_path);

  void DrawFileMenu(coreui::Project &frame);
  void DrawViewsMenu(coreui::Project &frame);

  OpenFileDialog open_file_dialog{};
  SaveAsFileDialog save_as_file_dialog{};

  // FamiliesView families_view{};
  // FlowTreeView flow_tree_view{};
  // GroupsView groups_view{};
  // GroupSettingsView group_settings_view{};
  SettingsView settings_view{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_MAIN_MENU_BAR_H_
