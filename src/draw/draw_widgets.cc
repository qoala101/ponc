#include "draw_widgets.h"

#include "draw_main_menu_bar.h"
#include "draw_node_editor.h"

namespace esc::draw {
void Widgets::Draw(const AppState &app_state) {
  main_menu_bar.Draw(app_state);
  node_editor.Draw(app_state);
}
}  // namespace esc::draw