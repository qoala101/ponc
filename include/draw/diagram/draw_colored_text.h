/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_COLORED_TEXT_H_
#define VH_PONC_DRAW_COLORED_TEXT_H_

#include <imgui.h>

#include <string_view>

namespace vh::ponc::draw {
///
enum class RelativePos { kTopLeft, kCenter };

///
void DrawColoredText(std::string_view text, const ImColor& color,
                     ImVec2 pos = ImGui::GetCursorPos(),
                     RelativePos relative_pos = RelativePos::kTopLeft);
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_COLORED_TEXT_H_
