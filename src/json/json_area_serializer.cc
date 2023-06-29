/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "json_area_serializer.h"

#include <crude_json.h>
#include <imgui_node_editor.h>

#include <memory>

#include "core_area.h"
#include "json_id_serializer.h"

namespace vh::ponc::json {
///
auto AreaSerializer::ParseFromJson(const crude_json::value& json)
    -> core::Area {
  return {
      .id = IdSerializer::ParseFromJson<ne::NodeId>(json["id"]),
      .name = json["name"].get<crude_json::string>(),
      .pos = {static_cast<float>(json["pos_x"].get<crude_json::number>()),
              static_cast<float>(json["pos_y"].get<crude_json::number>())},
      .size = {static_cast<float>(json["size_x"].get<crude_json::number>()),
               static_cast<float>(json["size_y"].get<crude_json::number>())}};
}

///
auto AreaSerializer::WriteToJson(const core::Area& area) -> crude_json::value {
  auto json = crude_json::value{};
  json["id"] = IdSerializer::WriteToJson(area.id);
  json["name"] = area.name;
  json["pos_x"] = area.pos.x;
  json["pos_y"] = area.pos.y;
  json["size_x"] = area.size.x;
  json["size_y"] = area.size.y;
  return json;
}
}  // namespace vh::ponc::json