/**
 * PON Calculator https://github.com/qoala101/ponc
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_JSON_SETINGS_SERIALIZER_H_
#define VH_PONC_JSON_SETINGS_SERIALIZER_H_

#include "core_settings.h"
#include "cpp_static_api.h"
#include "crude_json.h"

namespace vh::ponc::json {
///
struct SettingsSerializer : public cpp::StaticApi {
  ///
  static auto ParseFromJson(const crude_json::value &json) -> core::Settings;
  ///
  static auto WriteToJson(const core::Settings &settings) -> crude_json::value;
};
}  // namespace vh::ponc::json

#endif  // VH_PONC_JSON_SETINGS_SERIALIZER_H_
