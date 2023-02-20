/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_JSON_SETINGS_SERIALIZER_H_
#define VH_JSON_SETINGS_SERIALIZER_H_

#include "core_settings.h"
#include "crude_json.h"

namespace esc::json {
// ---
struct SettingsSerializer {
  // ---
  static auto ParseFromJson [[nodiscard]] (const crude_json::value &json)
  -> core::Settings;
  // ---
  static auto WriteToJson [[nodiscard]] (const core::Settings &settings)
  -> crude_json::value;
};
}  // namespace esc::json

#endif  // VH_JSON_SETINGS_SERIALIZER_H_
