/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#include "json_i_node_writer.h"

#include "core_i_node.h"
#include "crude_json.h"
#include "imgui_node_editor.h"
#include "json_id_serializer.h"

namespace esc::json {
// ---
auto INodeWriter::WriteToJson(const core::INode& node) const
    -> crude_json::value {
  auto json = crude_json::value{};

  json["id"] = IdSerializer::WriteToJson(node.GetId());
  json["family_id"] = IdSerializer::WriteToJson(node.GetFamilyId());

  const auto input_pin_id = node.GetInputPinId();

  if (input_pin_id.has_value()) {
    json["input_pin_id"] = IdSerializer::WriteToJson(*input_pin_id);
  }

  const auto& output_pin_ids = node.GetOutputPinIds();
  json["output_pin_ids_size"] =
      static_cast<crude_json::number>(output_pin_ids.size());
  auto& output_pin_ids_json = json["output_pin_ids"];

  for (auto i = 0; i < static_cast<int>(output_pin_ids.size()); ++i) {
    output_pin_ids_json[i] = IdSerializer::WriteToJson(output_pin_ids[i]);
  }

  const auto position = node.GetPosition();
  json["pos_x"] = position.x;
  json["pos_y"] = position.y;
  json["data"] = WriteToJson();

  return json;
}
}  // namespace esc::json