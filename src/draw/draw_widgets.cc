#include "draw_widgets.h"

#include "draw_node_editor.h"

namespace esc::draw {
Widgets::Widgets(const Texture &node_header_texture)
    : nodes{node_header_texture} {}

void Widgets::Draw(const AppState &app_state) {
  // draw::DrawMainWindow();
  node_editor.Draw(app_state);
}
}  // namespace esc::draw