#include "core_gradient.h"

#include "imgui.h"

namespace esc::core {
namespace {
///
auto GetGradient(float start_value, float end_value, float percentage) {
  return start_value + percentage * (end_value - start_value);
}
}  // namespace

///
auto GetGradient(const ImColor& start_color, const ImColor& end_color,
                 float percentage) -> ImColor {
  return ImColor{
      GetGradient(start_color.Value.x, end_color.Value.x, percentage),
      GetGradient(start_color.Value.y, end_color.Value.y, percentage),
      GetGradient(start_color.Value.z, end_color.Value.z, percentage)};
}
}  // namespace esc::core