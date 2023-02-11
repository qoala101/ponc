#include "json_i_node_writer.h"

#include "core_i_node.h"
#include "crude_json.h"
#include "imgui_node_editor.h"
#include "json_id_serializer.h"

namespace esc::json {
namespace {
void WritePinIds(const std::vector<ne::PinId>& pin_ids,
                 crude_json::value& json) {
  json["pin_ids_size"] = static_cast<crude_json::number>(pin_ids.size());
  auto& pin_ids_json = json["pin_ids"];

  for (auto i = 0; i < static_cast<int>(pin_ids.size()); ++i) {
    pin_ids_json[i] = IdSerializer::WriteToJson(pin_ids[i]);
  }
}
}  // namespace

auto INodeWriter::WriteToJson(const core::INode& node) const
    -> crude_json::value {
  auto json = crude_json::value{};

  json["id"] = IdSerializer::WriteToJson(node.GetId());
  WritePinIds(node.GetPinIds(), json);

  const auto position = node.GetPosition();
  json["pos_x"] = position.x;
  json["pos_y"] = position.y;
  json["data"] = WriteToJson();

  return json;
}
}  // namespace esc::json