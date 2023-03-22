#include "core_settings.h"
#include "crude_json.h"
#include "json_setings_serializer.h"

namespace vh::ponc::json {
///
auto SettingsSerializer::ParseFromJson(const crude_json::value& json)
    -> core::Settings {
  return {.color_flow = json["color_flow"].get<crude_json::boolean>(),
          .min_flow =
              static_cast<float>(json["min_flow"].get<crude_json::number>()),
          .low_flow =
              static_cast<float>(json["low_flow"].get<crude_json::number>()),
          .high_flow =
              static_cast<float>(json["high_flow"].get<crude_json::number>()),
          .max_flow =
              static_cast<float>(json["max_flow"].get<crude_json::number>())};
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
  return json;
}
}  // namespace vh::ponc::json