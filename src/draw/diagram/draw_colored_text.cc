/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_colored_text.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_node_editor.h>

#include "cpp_assert.h"

namespace ne = ax::NodeEditor;

namespace vh::ponc::draw {
///
void DrawColoredText(std::string_view text, const ImColor& color, ImVec2 pos,
                     RelativePos relative_pos) {
  const auto text_size = ImGui::CalcTextSize(text.data());
  auto rect = ImRect{ImVec2{}, text_size};

  const auto padding = ImGui::GetStyle().FramePadding;
  rect.Expand(padding);

  switch (relative_pos) {
    case RelativePos::kCenterLeft:
      pos.y -= rect.GetCenter().y;
      break;
    case RelativePos::kCenter:
      pos -= rect.GetCenter();
      break;
    default:
      break;
  }

  rect.Translate(pos);
  ImGui::SetCursorPos(pos);

  auto* draw_list = ImGui::GetWindowDrawList();
  Expects(draw_list != nullptr);

  draw_list->AddRectFilled(rect.Min, rect.Max, color,
                           ne::GetStyle().PinRounding);

  ImGui::TextUnformatted(text.data());
}
}  // namespace vh::ponc::draw