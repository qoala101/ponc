/**
 * PON Calculator https://github.com/qoala101/ponc
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_STYLE_UTILS_H_
#define VH_PONC_STYLE_UTILS_H_

#include "imgui.h"

namespace vh::ponc::style {
///
constexpr auto FromRgb(int r, int g, int b) {
  // NOLINTNEXTLINE(*-signed-bitwise)
  return IM_COL32(r, g, b, 255);
}

///
auto WithAlpha(ImColor color, float alpha) -> ImColor;
///
auto GetGradient(const ImColor& start_color, const ImColor& end_color,
                 float percentage) -> ImColor;
}  // namespace vh::ponc::style

#endif  // VH_PONC_STYLE_UTILS_H_
