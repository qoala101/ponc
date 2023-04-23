#include "calc_settings.h"

#include "core_i_node.h"

namespace vh::ponc::calc {
///
auto FamilySettings::FromFamilies(
    const std::vector<std::unique_ptr<core::IFamily>>& families)
    -> std::vector<FamilySettings> {
  auto family_settings = std::vector<FamilySettings>{};

  for (const auto& family : families) {
    if (const auto family_is_default = family->GetType().has_value()) {
      continue;
    }

    const auto sample_node = family->CreateSampleNode();

    if (const auto not_splitting =
            !sample_node->GetInputPinId().has_value() ||
            (sample_node->GetOutputPinIds().size() < 2)) {
      continue;
    }

    family_settings.emplace_back(FamilySettings{.family_id = family->GetId()});
  }

  return family_settings;
}

///
void CalculatorSettings::ResetToDefault(CalculatorSettings& settings) {
  settings.input = 6;
  settings.min_output = -22;
  settings.max_output = -18;
  settings.num_clients = 20;

  for (auto& family_settings : settings.family_settings) {
    family_settings.enabled = true;
    family_settings.cost = 100;
  }
}
}  // namespace vh::ponc::calc