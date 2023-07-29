/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "json_versifier.h"

#include <crude_json.h>
#include <imgui.h>

#include <algorithm>
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
void Upgrade2(crude_json::value& project_json) {
  auto& diagrams_json = project_json["diagrams"].get<crude_json::array>();

  for (auto& diagram_json : diagrams_json) {
    MakeArray(diagram_json["areas"]);

    auto& nodes_array = diagram_json["nodes"].get<crude_json::array>();

    for (auto& node_json : nodes_array) {
      const auto pos = ImVec2{
          static_cast<float>(node_json["pos_x"].get<crude_json::number>()),
          static_cast<float>(node_json["pos_y"].get<crude_json::number>())};
      node_json["pos"] = crude_json::array{pos.x, pos.y};
    }
  }
}

///
void Upgrade3(crude_json::value& project_json) {
  auto default_settings = core::Settings{};
  core::Settings::ResetToDefault(default_settings);

  auto& settings_json = project_json["settings"];
  settings_json["thick_links"] = default_settings.thick_links;
  settings_json["min_length"] = default_settings.min_length;
  settings_json["max_length"] = default_settings.max_length;

  auto& calculator_json = settings_json["calculator_settings"];
  calculator_json["num_clients"] =
      std::max(1.0, calculator_json["num_clients"].get<crude_json::number>());

  MakeArray(project_json["connections"]);

  auto& diagrams_json = project_json["diagrams"].get<crude_json::array>();

  for (auto& diagram_json : diagrams_json) {
    auto& links_array = diagram_json["links"].get<crude_json::array>();

    for (auto& link_json : links_array) {
      link_json["length"] = 0.F;
      link_json["connection"]["index"] = 0.F;
    }
  }
}

///
void Upgrade4(crude_json::value& /*unused*/) {
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
    case Version::kSlider:
      Upgrade2(project_json);
    case Version::kAreas:
      Upgrade3(project_json);
    case Version::kConnections:
      Upgrade4(project_json);
    default:
      break;
  }
}
}  // namespace vh::ponc::json