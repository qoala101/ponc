#ifndef VH_PONC_JSON_SETINGS_SERIALIZER_H_
#define VH_PONC_JSON_SETINGS_SERIALIZER_H_

#include "core_settings.h"
#include "cpp_no_instances.h"
#include "crude_json.h"

namespace vh::ponc::json {
///
struct SettingsSerializer : public cpp::NoInstances {
  ///
  static auto ParseFromJson(const crude_json::value &json) -> core::Settings;
  ///
  static auto WriteToJson(const core::Settings &settings) -> crude_json::value;
};
}  // namespace vh::ponc::json

#endif  // VH_PONC_JSON_SETINGS_SERIALIZER_H_
