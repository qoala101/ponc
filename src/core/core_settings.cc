#include "core_settings.h"

#include "core_gradient.h"

namespace vh::ponc::core {
///
auto Settings::GetFlowColor(const Settings &settings, float flow) -> ImColor {
  const auto blue = ImColor{0.F, 0.F, 1.F};

  if (flow < settings.min_flow) {
    return blue;
  }

  const auto red = ImColor{1.F, 0.F, 0.F};

  if (flow >= settings.max_flow) {
    return red;
  }

  const auto min_max_range = (settings.max_flow - settings.min_flow);
  const auto value_min_max_percentage =
      (flow - settings.min_flow) / min_max_range;
  const auto low_percentage =
      (settings.low_flow - settings.min_flow) / min_max_range;

  const auto blue_green = ImColor{0.F, 1.F, 1.F};

  if (value_min_max_percentage < low_percentage) {
    return GetGradient(blue, blue_green,
                       value_min_max_percentage / low_percentage);
  }

  const auto high_percentage =
      (settings.high_flow - settings.min_flow) / min_max_range;

  const auto green_red = ImColor{1.F, 1.F, 0.F};

  if (value_min_max_percentage >= high_percentage) {
    return GetGradient(
        green_red, red,
        (value_min_max_percentage - high_percentage) / (1.F - high_percentage));
  }

  const auto low_high_range = (high_percentage - low_percentage);
  const auto value_high_low_percentage =
      (value_min_max_percentage - low_percentage) / low_high_range;

  const auto green = ImColor{0.F, 1.F, 0.F};

  if (value_high_low_percentage < 0.5F) {
    return GetGradient(blue_green, green, value_high_low_percentage * 2);
  }

  return GetGradient(green, green_red, (value_high_low_percentage - 0.5F) * 2);
}
}  // namespace vh::ponc::core