#ifndef VH_JSON_SETINGS_SERIALIZER_H_
#define VH_JSON_SETINGS_SERIALIZER_H_

#include "core_settings.h"
#include "crude_json.h"

namespace esc::json {
///
struct SettingsSerializer {
  ///
  static auto ParseFromJson(const crude_json::value &json) -> core::Settings;
  ///
  static auto WriteToJson(const core::Settings &settings) -> crude_json::value;
};
}  // namespace esc::json

#endif  // VH_JSON_SETINGS_SERIALIZER_H_
