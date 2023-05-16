#include "core_settings.h"

#include "core_i_node.h"
#include "style_flow_colors.h"
#include "style_utils.h"

namespace vh::ponc::core {
///
auto CalculatorFamilySettings::FromFamilies(
    const std::vector<std::unique_ptr<core::IFamily>>& families)
    -> std::vector<CalculatorFamilySettings> {
  auto family_settings = std::vector<CalculatorFamilySettings>{};

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

    family_settings.emplace_back(
        CalculatorFamilySettings{.family_id = family->GetId()});
  }

  return family_settings;
}

///
void Settings::ResetToDefault(Settings& settings) {
  settings.color_flow = false;
  settings.min_flow = -27;
  settings.low_flow = -22;
  settings.high_flow = -18;
  settings.max_flow = 6;
  settings.arrange_horizontal_spacing = 80;
  settings.arrange_vertical_spacing = 20;

  settings.calculator_settings.min_output = -22;
  settings.calculator_settings.max_output = -18;
  settings.calculator_settings.num_clients = 20;

  for (auto& family_settings : settings.calculator_settings.family_settings) {
    family_settings.enabled = true;
    family_settings.cost = 100;
  }
}

///
auto Settings::GetFlowColor(const Settings& settings, float flow) -> ImColor {
  if (flow < settings.min_flow) {
    return style::FlowColors::kMin;
  }

  if (flow >= settings.max_flow) {
    return style::FlowColors::kMax;
  }

  const auto min_max_range = (settings.max_flow - settings.min_flow);
  const auto value_min_max_percentage =
      (flow - settings.min_flow) / min_max_range;
  const auto low_percentage =
      (settings.low_flow - settings.min_flow) / min_max_range;

  if (value_min_max_percentage < low_percentage) {
    return style::GetGradient(style::FlowColors::kMin, style::FlowColors::kLow,
                              value_min_max_percentage / low_percentage);
  }

  const auto high_percentage =
      (settings.high_flow - settings.min_flow) / min_max_range;

  if (value_min_max_percentage >= high_percentage) {
    return style::GetGradient(
        style::FlowColors::kHigh, style::FlowColors::kMax,
        (value_min_max_percentage - high_percentage) / (1 - high_percentage));
  }

  const auto low_high_range = (high_percentage - low_percentage);
  const auto value_high_low_percentage =
      (value_min_max_percentage - low_percentage) / low_high_range;

  if (value_high_low_percentage < 0.5) {
    return style::GetGradient(style::FlowColors::kLow, style::FlowColors::kGood,
                              value_high_low_percentage * 2);
  }

  return style::GetGradient(style::FlowColors::kGood, style::FlowColors::kHigh,
                            (value_high_low_percentage - 0.5F) * 2);
}
}  // namespace vh::ponc::core