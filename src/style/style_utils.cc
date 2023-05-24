/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "style_utils.h"

#include <imgui.h>

namespace vh::ponc::style {
namespace {
///
auto GetGradient(float start_value, float end_value, float percentage) {
  return start_value + percentage * (end_value - start_value);
}
}  // namespace

///
auto WithAlpha(ImColor color, float alpha) -> ImColor {
  color.Value.w = alpha;
  return color;
}

///
auto GetGradient(const ImColor& start_color, const ImColor& end_color,
                 float percentage) -> ImColor {
  return ImColor{
      GetGradient(start_color.Value.x, end_color.Value.x, percentage),
      GetGradient(start_color.Value.y, end_color.Value.y, percentage),
      GetGradient(start_color.Value.z, end_color.Value.z, percentage)};
}
}  // namespace vh::ponc::style