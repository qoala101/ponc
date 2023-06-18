/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "json_versifier.h"

#include <crude_json.h>

#include <type_traits>
#include <vector>

#include "core_i_family.h"
#include "core_settings.h"
#include "json_setings_serializer.h"

namespace vh::ponc::json {
namespace {
///
auto ParseVersion(const crude_json::value& json) {
  if (!json.contains("version")) {
    return Version::kPreCalculator;
  }

  return static_cast<Version>(json["version"].get<crude_json::number>());
}

///
void Upgrade0(crude_json::value& project_json) {
  auto settings = core::Settings{};

  auto& family_settings = settings.calculator_settings.family_settings;
  family_settings.reserve(16);

  for (auto family_id = 3; family_id <= 16; ++family_id) {
    family_settings.emplace_back(
        core::CalculatorFamilySettings{static_cast<core::FamilyId>(family_id)});
  }

  core::Settings::ResetToDefault(settings);

  auto& settings_json = project_json["settings"];

  settings.color_flow = settings_json["color_flow"].get<crude_json::boolean>();
  settings.min_flow =
      static_cast<float>(settings_json["min_flow"].get<crude_json::number>());
  settings.low_flow =
      static_cast<float>(settings_json["low_flow"].get<crude_json::number>());
  settings.high_flow =
      static_cast<float>(settings_json["high_flow"].get<crude_json::number>());
  settings.max_flow =
      static_cast<float>(settings_json["max_flow"].get<crude_json::number>());

  settings_json = SettingsSerializer::WriteToJson(settings);

  project_json["diagrams"][0]["name"] = "Diagram";
}

///
void MakeArray(crude_json::value& json) {
  if (json.is_null()) {
    json = crude_json::array{};
  }
}

///
void Upgrade1(crude_json::value& project_json) {
  auto& diagrams_json = project_json["diagrams"].get<crude_json::array>();

  for (auto& diagram_json : diagrams_json) {
    auto& nodes_json = diagram_json["nodes"];
    MakeArray(nodes_json);

    auto& nodes_array = nodes_json.get<crude_json::array>();

    for (auto& node_json : nodes_array) {
      MakeArray(node_json["output_pin_ids"]);
    }

    MakeArray(diagram_json["links"]);
  }
}

///
void Upgrade2(crude_json::value& /*unused*/) {
  // vh: Implement when adding new version.
}
}  // namespace

///
auto Versifier::GetCurrentVersion() -> Version {
  using T = std::underlying_type_t<Version>;

  return static_cast<Version>(static_cast<T>(Version::kAfterCurrent) - 1);
}

///
void Versifier::UpgradeToCurrentVersion(crude_json::value& project_json) {
  const auto json_version = ParseVersion(project_json);
  const auto current_version = GetCurrentVersion();

  if (json_version >= current_version) {
    return;
  }

  switch (json_version) {
    case Version::kPreCalculator:
      Upgrade0(project_json);
    case Version::kCalculator:
      Upgrade1(project_json);
    case Version::kAreas:
      Upgrade2(project_json);
    default:
      break;
  }
}
}  // namespace vh::ponc::json