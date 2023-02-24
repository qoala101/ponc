/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#include "json_i_node_parser.h"

#include "core_family_id.h"
#include "core_i_node.h"
#include "crude_json.h"
#include "imgui_node_editor.h"
#include "json_id_serializer.h"

namespace esc::json {
auto INodeParser::ParseFromJson(const crude_json::value& json) const
    -> std::shared_ptr<core::INode> {
  auto parsed_args = core::INode::ConstructorArgs{
      .id = IdSerializer::ParseFromJson<ne::NodeId>(json["id"]),
      .family_id =
          IdSerializer::ParseFromJson<core::FamilyId>(json["family_id"])};

  if (json.contains("input_pin_id")) {
    parsed_args.input_pin_id =
        IdSerializer::ParseFromJson<ne::PinId>(json["input_pin_id"]);
  }

  const auto output_pin_ids_size =
      json["output_pin_ids_size"].get<crude_json::number>();
  const auto& output_pin_ids_json = json["output_pin_ids"];

  for (auto i = 0; i < output_pin_ids_size; ++i) {
    parsed_args.output_pin_ids.emplace_back(
        IdSerializer::ParseFromJson<ne::PinId>(output_pin_ids_json[i]));
  }

  auto parsed_node = ParseFromJson(std::move(parsed_args), json["data"]);

  const auto parsed_position =
      ImVec2{static_cast<float>(json["pos_x"].get<crude_json::number>()),
             static_cast<float>(json["pos_y"].get<crude_json::number>())};
  parsed_node->SetPosition(parsed_position);

  return parsed_node;
}
}  // namespace esc::json