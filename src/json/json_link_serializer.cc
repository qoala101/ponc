#include "json_link_serializer.h"

#include <imgui_node_editor.h>

#include <memory>

#include "core_link.h"
#include "crude_json.h"
#include "json_id_serializer.h"

namespace esc::json {
auto LinkSerializer::ParseFromJson(const crude_json::value& json)
    -> core::Link {
  return core::Link{
      IdSerializer::ParseFromJson<ne::LinkId>(json["id"]),
      IdSerializer::ParseFromJson<ne::PinId>(json["start_pin_id"]),
      IdSerializer::ParseFromJson<ne::PinId>(json["end_pin_id"])};
}

auto LinkSerializer::WriteToJson(const core::Link& link) -> crude_json::value {
  auto json = crude_json::value{};
  json["id"] = IdSerializer::WriteToJson(link.id);
  json["start_pin_id"] = IdSerializer::WriteToJson(link.start_pin_id);
  json["end_pin_id"] = IdSerializer::WriteToJson(link.end_pin_id);
  return json;
}
}  // namespace esc::json