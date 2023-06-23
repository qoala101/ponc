/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include <crude_json.h>

#include <algorithm>
#include <memory>

#include "core_i_family.h"
#include "core_settings.h"
#include "json_container_serializer.h"
#include "json_id_serializer.h"
#include "json_setings_serializer.h"

namespace vh::ponc::json {
namespace {
///
auto ParseFamilySettingsFromJson(const crude_json::value& json) {
  return core::CalculatorFamilySettings{
      .family_id =
          IdSerializer::ParseFromJson<core::FamilyId>(json["family_id"]),
      .enabled = json["enabled"].get<crude_json::boolean>(),
      .cost = static_cast<float>(json["cost"].get<crude_json::number>())};
}

///
auto WriteFamilySettingsToJson(const core::CalculatorFamilySettings& settings) {
  auto json = crude_json::value{};
  json["family_id"] = IdSerializer::WriteToJson(settings.family_id);
  json["enabled"] = settings.enabled;
  json["cost"] = settings.cost;
  return json;
}
}  // namespace

///
auto SettingsSerializer::ParseFromJson(const crude_json::value& json)
    -> core::Settings {
  const auto& calculator_json = json["calculator_settings"];

  return {.color_flow = json["color_flow"].get<crude_json::boolean>(),
          .min_flow =
              static_cast<float>(json["min_flow"].get<crude_json::number>()),
          .low_flow =
              static_cast<float>(json["low_flow"].get<crude_json::number>()),
          .high_flow =
              static_cast<float>(json["high_flow"].get<crude_json::number>()),
          .max_flow =
              static_cast<float>(json["max_flow"].get<crude_json::number>()),
          .arrange_horizontal_spacing = static_cast<int>(
              json["arrange_horizontal_spacing"].get<crude_json::number>()),
          .arrange_vertical_spacing = static_cast<int>(
              json["arrange_vertical_spacing"].get<crude_json::number>()),
          .calculator_settings = {
              .min_output = static_cast<float>(
                  calculator_json["min_output"].get<crude_json::number>()),
              .max_output = static_cast<float>(
                  calculator_json["max_output"].get<crude_json::number>()),
              .num_clients = static_cast<int>(
                  calculator_json["num_clients"].get<crude_json::number>()),
              .family_settings = ContainerSerializer::ParseFromJson<
                  core::CalculatorFamilySettings>(
                  calculator_json["family_settings"],
                  &ParseFamilySettingsFromJson)}};
}

///
auto SettingsSerializer::WriteToJson(const core::Settings& settings)
    -> crude_json::value {
  auto json = crude_json::value{};
  json["color_flow"] = settings.color_flow;
  json["min_flow"] = settings.min_flow;
  json["low_flow"] = settings.low_flow;
  json["high_flow"] = settings.high_flow;
  json["max_flow"] = settings.max_flow;
  json["arrange_horizontal_spacing"] =
      static_cast<crude_json::number>(settings.arrange_horizontal_spacing);
  json["arrange_vertical_spacing"] =
      static_cast<crude_json::number>(settings.arrange_vertical_spacing);

  auto& calculator_json = json["calculator_settings"];
  calculator_json["min_output"] = settings.calculator_settings.min_output;
  calculator_json["max_output"] = settings.calculator_settings.max_output;
  calculator_json["num_clients"] =
      static_cast<crude_json::number>(settings.calculator_settings.num_clients);

  calculator_json["family_settings"] = ContainerSerializer::WriteToJson(
      settings.calculator_settings.family_settings, &WriteFamilySettingsToJson);

  return json;
}
}  // namespace vh::ponc::json