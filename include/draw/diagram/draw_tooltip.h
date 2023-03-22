#ifndef VH_PONC_DRAW_TOOLTIP_H_
#define VH_PONC_DRAW_TOOLTIP_H_

#include <imgui.h>

#include <string_view>

namespace vh::ponc::draw {
///
void DrawTooltip(std::string_view text, const ImColor& color);
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_TOOLTIP_H_
