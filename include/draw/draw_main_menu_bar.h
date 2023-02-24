/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_DRAW_MAIN_MENU_BAR_H_
#define VH_DRAW_MAIN_MENU_BAR_H_

#include "app_state.h"
#include "draw_open_file_dialog.h"
#include "draw_save_as_file_dialog.h"
#include "draw_settings_view.h"

namespace esc::draw {
class MainMenuBar {
 public:
  void Draw(const AppState& app_state);

 private:
  void DrawFileMenu(const AppState& app_state);
  void DrawViewsMenu(const AppState& app_state);

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
