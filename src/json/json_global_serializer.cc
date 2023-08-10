/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "json_global_serializer.h"

#include <crude_json.h>
#include <imgui.h>

#include <memory>

#include "app_globals.h"

namespace vh::ponc::json {
///
auto GlobalSerializer::ParseFromJson(const crude_json::value& json)
    -> std::pair<std::string, Global> {
  return {};
}

///
auto GlobalSerializer::WriteToJson(const std::pair<std::string, Global>& global)
    -> crude_json::value {
  auto json = crude_json::value{};
  std::visit([&json, &global](const auto& v) { json[global.first] = v; },
             global.second);
  return json;
}
}  // namespace vh::ponc::json