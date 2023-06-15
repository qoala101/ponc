/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_tags.h"

#include <imgui.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#include <imgui_node_editor.h>

namespace vh::ponc::draw {
void DrawTags(const coreui::Tags& tags) {
  const auto pos = ImGui::GetMousePos();
  ImGui::SetCursorPos(pos);

  auto rect = ImRect{{}, {100, 100}};

  const auto cursor_screen_pos = ImGui::GetCursorScreenPos();
  rect.Translate(cursor_screen_pos);

  const auto color = ImColor{255, 0, 0};
  auto* drawList = ImGui::GetWindowDrawList();
  drawList->AddRectFilled(rect.Min, rect.Max, color,
                          ne::GetStyle().PinRounding);
}
}  // namespace vh::ponc::draw