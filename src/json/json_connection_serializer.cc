/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "json_connection_serializer.h"

#include <crude_json.h>

#include <memory>

#include "core_connection.h"
#include "json_color_serializer.h"
#include "json_id_serializer.h"

namespace vh::ponc::json {
///
auto ConnectionSerializer::ParseFromJson(const crude_json::value& json)
    -> core::Connection {
  return {.id = IdSerializer::ParseFromJson<core::ConnectionId>(json["id"]),
          .name = json["name"].get<crude_json::string>(),
          .color = ColorSerializer::ParseFromJson(json["color"]),
          .drop_per_length = static_cast<float>(
              json["drop_per_length"].get<crude_json::number>()),
          .drop_added =
              static_cast<float>(json["drop_added"].get<crude_json::number>())};
}

///
auto ConnectionSerializer::WriteToJson(const core::Connection& connection)
    -> crude_json::value {
  auto json = crude_json::value{};
  json["id"] = IdSerializer::WriteToJson(connection.id);
  json["name"] = connection.name;
  json["color"] = ColorSerializer::WriteToJson(connection.color);
  json["drop_per_length"] = connection.drop_per_length;
  json["drop_added"] = connection.drop_added;
  return json;
}
}  // namespace vh::ponc::json