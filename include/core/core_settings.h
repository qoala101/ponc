#ifndef VH_PONC_CORE_SETTINGS_H_
#define VH_PONC_CORE_SETTINGS_H_

#include <imgui.h>

#include "calc_settings.h"

namespace vh::ponc::core {
///
struct Settings {
  ///
  static auto GetFlowColor(const Settings &settings, float flow) -> ImColor;

  ///
  bool color_flow{};
  ///
  float min_flow{};
  ///
  float low_flow{};
  ///
  float high_flow{};
  ///
  float max_flow{};
  ///
  calc::CalculatorSettings calculator_settings{};
};
}  // namespace vh::ponc::core

#endif  // VH_PONC_CORE_SETTINGS_H_
