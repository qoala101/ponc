/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_links.h"

#include <imgui.h>
#include <imgui_node_editor.h>
#include <imgui_node_editor_internal.h>

#include <charconv>
#include <cstdlib>

#include "core_link.h"
#include "coreui_link.h"
#include "coreui_native_facade.h"
#include "cpp_assert.h"
#include "draw_colored_text.h"
#include "draw_string_buffer.h"

namespace vh::ponc::draw {
///
void Links::Draw(const std::vector<coreui::Link> &links) {
  for (const auto &link : links) {
    ne::Link(link.core_link.id, link.core_link.start_pin_id,
             link.core_link.end_pin_id, link.color, link.thickness);
    DrawLinkDrop(link);
  }
}

///
void Links::DrawLinkDrop(const coreui::Link &link) {
  if (link.drop >= 0) {
    return;
  }

  ImFormatString(drop_buffer_.GetData(), drop_buffer_.GetSize(), "%.2f",
                 link.drop);

  const auto &style = ne::GetStyle();
  const auto color = style.Colors[ne::StyleColor_NodeBg];
  const auto curve = coreui::NativeFacade::GetLinkCurve(link.core_link.id);
  const auto padding = style.NodePadding.x * 2;
  const auto pos = curve.P0 + ImVec2{padding, 0};

  DrawColoredText(drop_buffer_.GetData(), color, pos, RelativePos::kCenterLeft);
}
}  // namespace vh::ponc::draw