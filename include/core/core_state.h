#ifndef VH_CORE_STATE_H_
#define VH_CORE_STATE_H_

#include "core_diagram.h"
#include "flow_calculator.h"

namespace esc::core {
struct FlowColors {
  bool color_flow{};
  float min{-27};
  float low{-22};
  float high{-18};
  float max{6};

  auto GetColorForFlowValue(float value) const -> ImColor {
    if (!color_flow) {
      return ImColor{255, 255, 255};
    }

    const auto blue = ImColor{0, 0, 255};
    const auto blue_green = ImColor{0, 255, 255};
    const auto green = ImColor{0, 255, 0};
    const auto green_red = ImColor{255, 255, 0};
    const auto red = ImColor{255, 0, 0};

    if (value < min) {
      return blue;
    }

    if (value >= max) {
      return red;
    }

    const auto range = (max - min);
    const auto value_percentage = (value - min) / range;
    const auto low_percentage = (low - min) / range;
    const auto high_percentage = (high - min) / range;

    auto percentage = 0.0F;
    auto start_color = ImColor{};
    auto end_color = ImColor{};

    if (value_percentage < low_percentage) {
      percentage = value_percentage / low_percentage;
      start_color = blue;
      end_color = blue_green;
    } else if (value_percentage >= high_percentage) {
      percentage =
          (value_percentage - high_percentage) / (1.0F - high_percentage);
      start_color = green_red;
      end_color = red;
    } else {
      const auto low_high_range = (high_percentage - low_percentage);
      percentage = (value_percentage - low_percentage) / low_high_range;

      if (percentage < 0.5F) {
        percentage = percentage * 2;
        start_color = blue_green;
        end_color = green;
      } else {
        percentage = (percentage - 0.5F) * 2;
        start_color = green;
        end_color = green_red;
      }
    }

    return ImColor{start_color.Value.x +
                       percentage * (end_color.Value.x - start_color.Value.x),
                   start_color.Value.y +
                       percentage * (end_color.Value.y - start_color.Value.y),
                   start_color.Value.z +
                       percentage * (end_color.Value.z - start_color.Value.z)};
  }
};

struct CoreState {
  Diagram diagram_{};
  IdGenerator id_generator_{};
  flow::FlowCalculator flow_calculator_{};
  FlowColors flow_colors_{};
};
}  // namespace esc::core

#endif  // VH_CORE_STATE_H_
