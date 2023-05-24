/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_link.h"

#include <imgui.h>
#include <imgui_node_editor.h>

#include "core_link.h"
#include "coreui_link.h"

namespace vh::ponc::draw {
///
void DrawLink(const coreui::Link& link) {
  ne::Link(link.core_link.id, link.core_link.start_pin_id,
           link.core_link.end_pin_id, link.color, link.thickness);
}
}  // namespace vh::ponc::draw