/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_JSON_GLOBAL_SERIALIZER_H_
#define VH_PONC_JSON_GLOBAL_SERIALIZER_H_

#include <crude_json.h>

#include <string>
#include <utility>

#include "app_globals.h"
#include "cpp_static_api.h"

namespace vh::ponc::json {
///
struct GlobalSerializer : public cpp::StaticApi {
  ///
  static auto ParseFromJson(const crude_json::value &json)
      -> std::pair<std::string, Global>;
  ///
  static auto WriteToJson(const std::pair<std::string, Global> &global)
      -> crude_json::value;
};
}  // namespace vh::ponc::json

#endif  // VH_PONC_JSON_GLOBAL_SERIALIZER_H_
