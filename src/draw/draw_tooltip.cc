#include "draw_tooltip.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include <utility>

namespace esc::draw {
// ---
Tooltip::Tooltip(std::string text, const ImColor& color)
    : text_{std::move(text)}, color_{color} {}

// ---
void Tooltip::Draw(State& state) {
  const auto& style = ImGui::GetStyle();
  const auto spacing = style.ItemSpacing;
  const auto padding = style.FramePadding;

  ImGui::SetCursorPos(
      ImGui::GetCursorPos() +
      ImVec2{spacing.x, -spacing.y - ImGui::GetTextLineHeight()});

  const auto cursor_screen_pos = ImGui::GetCursorScreenPos();
  const auto text_size = ImGui::CalcTextSize(text_.c_str());
  const auto rect = ImRect{cursor_screen_pos - padding,
                           cursor_screen_pos + text_size + padding};

  auto* drawList = ImGui::GetWindowDrawList();
  drawList->AddRectFilled(rect.Min, rect.Max, color_, text_size.y * 0.2F);

  ImGui::TextUnformatted(text_.c_str());
}
}  // namespace esc::draw