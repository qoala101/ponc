/**
 * PON Calculator https://github.com/qoala101/ponc
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_tooltip.h"

#include "draw_colored_text.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include <utility>

namespace vh::ponc::draw {
///
void DrawTooltip(std::string_view text, const ImColor& color) {
  const auto spacing = ImGui::GetStyle().ItemSpacing;
  const auto pos = ImGui::GetCursorPos() +
                   ImVec2{spacing.x, -spacing.y - ImGui::GetTextLineHeight()};
  DrawColoredText(text, color, pos);
}
}  // namespace vh::ponc::draw