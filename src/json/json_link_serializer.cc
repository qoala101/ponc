/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "json_link_serializer.h"

#include <imgui_node_editor.h>

#include <memory>

#include "core_link.h"
#include "crude_json.h"
#include "json_id_serializer.h"

namespace vh::ponc::json {
///
auto LinkSerializer::ParseFromJson(const crude_json::value& json)
    -> core::Link {
  return {
      .id = IdSerializer::ParseFromJson<ne::LinkId>(json["id"]),
      .start_pin_id =
          IdSerializer::ParseFromJson<ne::PinId>(json["start_pin_id"]),
      .end_pin_id = IdSerializer::ParseFromJson<ne::PinId>(json["end_pin_id"])};
}

///
auto LinkSerializer::WriteToJson(const core::Link& link) -> crude_json::value {
  auto json = crude_json::value{};
  json["id"] = IdSerializer::WriteToJson(link.id);
  json["start_pin_id"] = IdSerializer::WriteToJson(link.start_pin_id);
  json["end_pin_id"] = IdSerializer::WriteToJson(link.end_pin_id);
  return json;
}
}  // namespace vh::ponc::json