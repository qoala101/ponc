#include "core_settings.h"
#include "crude_json.h"
#include "json_setings_serializer.h"

namespace esc::json {
auto SettingsSerializer::ParseFromJson(const crude_json::value& json)
    -> core::Settings {
  return core::Settings{};
}

auto SettingsSerializer::WriteToJson(const core::Settings& settings)
    -> crude_json::value {
  auto json = crude_json::value{};
  return json;
}
}  // namespace esc::json