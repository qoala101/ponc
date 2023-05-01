#ifndef VH_PONC_CORE_SETTINGS_H_
#define VH_PONC_CORE_SETTINGS_H_

#include <imgui.h>

#include "core_i_family.h"

namespace vh::ponc::core {
///
struct CalculatorFamilySettings {
  ///
  static auto FromFamilies(
      const std::vector<std::unique_ptr<core::IFamily>> &families)
      -> std::vector<CalculatorFamilySettings>;

  ///
  core::FamilyId family_id{};
  ///
  bool enabled{};
  ///
  float cost{};
};

///
struct CalculatorSettings {
  ///
  float min_output{};
  ///
  float max_output{};
  ///
  int num_clients{};
  ///
  std::vector<CalculatorFamilySettings> family_settings{};
};

///
struct Settings {
  ///
  static void ResetToDefault(Settings &settings);
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
  int arrange_horizontal_spacing{};
  ///
  int arrange_vertical_spacing{};
  ///
  CalculatorSettings calculator_settings{};
};
}  // namespace vh::ponc::core

#endif  // VH_PONC_CORE_SETTINGS_H_
