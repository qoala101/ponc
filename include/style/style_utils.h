#ifndef VH_PONC_STYLE_UTILS_H_
#define VH_PONC_STYLE_UTILS_H_

#include "imgui.h"

namespace vh::ponc::style {
///
auto WithAlpha(ImColor color, float alpha) -> ImColor;
///
auto GetGradient(const ImColor& start_color, const ImColor& end_color,
                 float percentage) -> ImColor;
}  // namespace vh::ponc::style

#endif  // VH_PONC_STYLE_UTILS_H_
