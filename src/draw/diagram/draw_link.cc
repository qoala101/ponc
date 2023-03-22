#include "draw_link.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <variant>

#include "core_link.h"
#include "coreui_link.h"
#include "imgui_node_editor.h"

namespace vh::ponc::draw {
///
void DrawLink(const coreui::Link& link) {
  ne::Link(link.core_link.id, link.core_link.start_pin_id,
           link.core_link.end_pin_id, link.color, link.thickness);
}
}  // namespace vh::ponc::draw