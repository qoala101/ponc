/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "json_color_serializer.h"

#include <crude_json.h>
#include <imgui.h>

namespace vh::ponc::json {
///
auto ColorSerializer::ParseFromJson(const crude_json::value& json) -> ImColor {
  return {static_cast<float>(json[0].get<crude_json::number>()),
          static_cast<float>(json[1].get<crude_json::number>()),
          static_cast<float>(json[2].get<crude_json::number>()),
          static_cast<float>(json[3].get<crude_json::number>())};
}

///
auto ColorSerializer::WriteToJson(const ImColor& color) -> crude_json::value {
  auto json = crude_json::array{};
  json.emplace_back(color.Value.x);
  json.emplace_back(color.Value.y);
  json.emplace_back(color.Value.z);
  json.emplace_back(color.Value.w);
  return json;
}
}  // namespace vh::ponc::json