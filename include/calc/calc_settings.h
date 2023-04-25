#ifndef VH_PONC_CALC_SETTINGS_H_
#define VH_PONC_CALC_SETTINGS_H_

#include "core_i_family.h"

namespace vh::ponc::calc {
///
struct FamilySettings {
  ///
  static auto FromFamilies(
      const std::vector<std::unique_ptr<core::IFamily>> &families)
      -> std::vector<FamilySettings>;

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
  static void ResetToDefault(CalculatorSettings &settings);

  ///
  float min_output{};
  ///
  float max_output{};
  ///
  int num_clients{};
  ///
  std::vector<FamilySettings> family_settings{};
};
}  // namespace vh::ponc::calc

#endif  // VH_PONC_CALC_SETTINGS_H_
