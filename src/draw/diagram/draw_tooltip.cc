#include "draw_tooltip.h"

#include "draw_colored_text.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include <utility>

namespace esc::draw {
///
void DrawTooltip(std::string_view text, const ImColor& color) {
  const auto spacing = ImGui::GetStyle().ItemSpacing;
  const auto pos = ImGui::GetCursorPos() +
                   ImVec2{spacing.x, -spacing.y - ImGui::GetTextLineHeight()};
  DrawColoredText(text, color, pos);
}
}  // namespace esc::draw