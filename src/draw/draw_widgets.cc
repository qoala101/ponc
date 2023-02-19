#include "draw_widgets.h"

#include "draw_node_editor.h"

namespace esc::draw {
Widgets::Widgets(const Texture &node_header_texture)
// : nodes{node_header_texture}
{}

void Widgets::Draw(core::Project &project, core::IdGenerator &id_generator,
                   flow::FlowCalculator &flow_calculator,
                   EventQueue &event_queue) {
  // draw::DrawMainWindow();
  node_editor.Draw();
}
}  // namespace esc::draw