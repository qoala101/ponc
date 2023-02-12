#ifndef VH_DRAW_MENU_BAR_H_
#define VH_DRAW_MENU_BAR_H_

#include "draw_families_view.h"
#include "draw_flow_tree_view.h"
#include "draw_i_drawable.h"
#include "draw_open_file_dialog.h"
#include "draw_save_as_file_dialog.h"
#include "esc_state.h"

namespace esc::draw {
// ---
class MenuBar : public IDrawable {
 public:
  // ---
  void Draw(State &state) override;

 private:
  OpenFileDialog open_file_dialog_;
  SaveAsFileDialog save_as_file_dialog_;
  FamiliesView families_view_{};
  FlowTreeView flow_tree_view_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_MENU_BAR_H_
