/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "json_color_serializer.h"

#include <crude_json.h>
#include <imgui.h>
#include <imgui_node_editor.h>

namespace vh::ponc::json {
///
auto ColorSerializer::ParseFromJson(const crude_json::value& json) -> ImColor {
  return {static_cast<float>(json["r"].get<crude_json::number>()),
          static_cast<float>(json["g"].get<crude_json::number>()),
          static_cast<float>(json["b"].get<crude_json::number>()),
          static_cast<float>(json["a"].get<crude_json::number>())};
}

///
auto ColorSerializer::WriteToJson(const ImColor& color) -> crude_json::value {
  auto json = crude_json::value{};
  json["r"] = color.Value.x;
  json["g"] = color.Value.y;
  json["b"] = color.Value.z;
  json["a"] = color.Value.w;
  return json;
}
}  // namespace vh::ponc::json