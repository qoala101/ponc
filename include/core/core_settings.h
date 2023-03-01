#ifndef VH_CORE_SETTINGS_H_
#define VH_CORE_SETTINGS_H_

#include "imgui.h"

namespace esc::core {
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
};
}  // namespace esc::core

#endif  // VH_CORE_SETTINGS_H_
