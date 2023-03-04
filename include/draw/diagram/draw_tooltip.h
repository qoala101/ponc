#ifndef VH_DRAW_TOOLTIP_H_
#define VH_DRAW_TOOLTIP_H_

#include <imgui.h>

#include <string_view>

namespace esc::draw {
///
void DrawTooltip(std::string_view text, const ImColor& color);
}  // namespace esc::draw

#endif  // VH_DRAW_TOOLTIP_H_
