/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "json_area_serializer.h"

#include <crude_json.h>
#include <imgui.h>

#include <memory>

#include "core_area.h"
#include "json_id_serializer.h"

namespace vh::ponc::json {
///
auto AreaSerializer::ParseFromJson(const crude_json::value& json)
    -> core::Area {
  const auto& pos = json["pos"].get<crude_json::array>();
  const auto& size = json["size"].get<crude_json::array>();

  return {.id = IdSerializer::ParseFromJson<core::AreaId>(json["id"]),
          .name = json["name"].get<crude_json::string>(),
          .pos = {static_cast<float>(pos[0].get<crude_json::number>()),
                  static_cast<float>(pos[1].get<crude_json::number>())},
          .size = {static_cast<float>(size[0].get<crude_json::number>()),
                   static_cast<float>(size[1].get<crude_json::number>())}};
}

///
auto AreaSerializer::WriteToJson(const core::Area& area) -> crude_json::value {
  auto json = crude_json::value{};
  json["id"] = IdSerializer::WriteToJson(area.id);
  json["name"] = area.name;
  json["pos"] = crude_json::array{area.pos.x, area.pos.y};
  json["size"] = crude_json::array{area.size.x, area.size.y};
  return json;
}
}  // namespace vh::ponc::json