#ifndef VH_DRAW_MENU_BAR_H_
#define VH_DRAW_MENU_BAR_H_

#include "draw_families_view.h"
#include "draw_i_drawable.h"
#include "draw_nodes_view.h"
#include "draw_open_file_dialog.h"
#include "draw_save_as_file_dialog.h"
#include "esc_app_state.h"

namespace esc::draw {
// ---
class MenuBar : public IDrawable {
 public:
  // ---
  void Draw(AppState &app_state) override;

 private:
  OpenFileDialog open_file_dialog_;
  SaveAsFileDialog save_as_file_dialog_;
  NodesView nodes_view_{};
  FamiliesView families_view_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_MENU_BAR_H_
