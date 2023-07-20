/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "json_link_serializer.h"

#include <crude_json.h>
#include <imgui_node_editor.h>

#include <memory>
#include <type_traits>
#include <variant>

#include "core_connection.h"
#include "core_link.h"
#include "cpp_assert.h"
#include "json_color_serializer.h"
#include "json_id_serializer.h"

namespace vh::ponc::json {
namespace {
///
auto ParseConnection(const crude_json::value& json) -> core::LinkConnection {
  const auto index = static_cast<int>(json["index"].get<crude_json::number>());

  if (index == 0) {
    return {};
  }

  const auto& value_json = json["value"];

  switch (index) {
    case 1:
      return IdSerializer::ParseFromJson<core::ConnectionId>(value_json);

    case 2:
      return core::CustomConnection{
          .color = ColorSerializer::ParseFromJson(value_json["color"]),
          .drop_per_length = static_cast<float>(
              value_json["drop_per_length"].get<crude_json::number>()),
          .drop_added = static_cast<float>(
              value_json["drop_added"].get<crude_json::number>())};

    default:
      break;
  }

  Expects(false);
}

///
auto WriteConnection(const core::LinkConnection& connection) {
  auto json = crude_json::value{};
  json["index"] = static_cast<crude_json::number>(connection.index());

  std::visit(
      [&json](const auto& v) {
        using V = std::remove_cvref_t<decltype(v)>;

        if constexpr (std::is_same_v<V, core::ConnectionId>) {
          json["value"] = IdSerializer::WriteToJson(v);
        } else if constexpr (std::is_same_v<V, core::CustomConnection>) {
          auto& value_json = json["value"];
          value_json["color"] = ColorSerializer::WriteToJson(v.color);
          value_json["drop_per_length"] = v.drop_per_length;
          value_json["drop_added"] = v.drop_added;
        }
      },
      connection);

  return json;
}
}  // namespace

///
auto LinkSerializer::ParseFromJson(const crude_json::value& json)
    -> core::Link {
  return {
      .id = IdSerializer::ParseFromJson<ne::LinkId>(json["id"]),
      .start_pin_id =
          IdSerializer::ParseFromJson<ne::PinId>(json["start_pin_id"]),
      .end_pin_id = IdSerializer::ParseFromJson<ne::PinId>(json["end_pin_id"]),
      .length = static_cast<float>(json["length"].get<crude_json::number>()),
      .connection = ParseConnection(json["connection"])};
}

///
auto LinkSerializer::WriteToJson(const core::Link& link) -> crude_json::value {
  auto json = crude_json::value{};
  json["id"] = IdSerializer::WriteToJson(link.id);
  json["start_pin_id"] = IdSerializer::WriteToJson(link.start_pin_id);
  json["end_pin_id"] = IdSerializer::WriteToJson(link.end_pin_id);
  json["length"] = link.length;
  json["connection"] = WriteConnection(link.connection);
  return json;
}
}  // namespace vh::ponc::json