#include "draw_colored_text.h"

#include "imgui.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#include <imgui_node_editor.h>

#include <utility>

namespace ne = ax::NodeEditor;

namespace esc::draw {
///
void DrawColoredText(std::string_view text, const ImColor& color,
                     const ImVec2& pos) {
  ImGui::SetCursorPos(pos);

  const auto cursor_screen_pos = ImGui::GetCursorScreenPos();
  const auto text_size = ImGui::CalcTextSize(text.data());
  const auto padding = ImGui::GetStyle().FramePadding;
  const auto rect = ImRect{cursor_screen_pos - padding,
                           cursor_screen_pos + text_size + padding};

  auto* drawList = ImGui::GetWindowDrawList();
  drawList->AddRectFilled(rect.Min, rect.Max, color,
                          ne::GetStyle().PinRounding);
  ImGui::TextUnformatted(text.data());
}
}  // namespace esc::draw