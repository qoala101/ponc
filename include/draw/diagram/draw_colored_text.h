#ifndef VH_DRAW_COLORED_TEXT_H_
#define VH_DRAW_COLORED_TEXT_H_

#include <imgui.h>

#include <string_view>

namespace esc::draw {
///
void DrawColoredText(std::string_view text, const ImColor& color,
                     const ImVec2& pos = ImGui::GetCursorPos());
}  // namespace esc::draw

#endif  // VH_DRAW_COLORED_TEXT_H_
