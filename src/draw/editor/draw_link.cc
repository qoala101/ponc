#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>

#include "cpp_assert.h"
#include "draw_link.h"
#include "frame_node.h"
#include "imgui_bezier_math.h"
#include "imgui_node_editor.h"

namespace esc::draw {
void DrawLink(const coreui::Link& link) {
  ne::Link(link.id, link.start_pin, link.end_pin, link.color, link.thickness);
}
}  // namespace esc::draw