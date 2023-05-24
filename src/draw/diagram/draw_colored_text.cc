/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_colored_text.h"

#include <imgui.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#include <imgui_node_editor.h>

namespace ne = ax::NodeEditor;

namespace vh::ponc::draw {
///
void DrawColoredText(std::string_view text, const ImColor& color,
                     const ImVec2& pos) {
  ImGui::SetCursorPos(pos);

  const auto cursor_screen_pos = ImGui::GetCursorScreenPos();
  const auto text_size = ImGui::CalcTextSize(text.data());
  const auto padding = ImGui::GetStyle().FramePadding;

  auto rect = ImRect{ImVec2{} - padding, text_size + padding};
  rect.Translate(cursor_screen_pos);

  auto* drawList = ImGui::GetWindowDrawList();
  drawList->AddRectFilled(rect.Min, rect.Max, color,
                          ne::GetStyle().PinRounding);

  ImGui::TextUnformatted(text.data());
}
}  // namespace vh::ponc::draw