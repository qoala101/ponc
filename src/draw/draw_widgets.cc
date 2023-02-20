#include "draw_widgets.h"

#include "draw_main_menu_bar.h"
#include "draw_node_editor.h"

namespace esc::draw {
Widgets::Widgets(const Texture &node_header_texture)
    : nodes{node_header_texture} {}

void Widgets::Draw(const AppState &app_state) {
  draw::DrawMainMenuBar(app_state);
  node_editor.Draw(app_state);
}
}  // namespace esc::draw