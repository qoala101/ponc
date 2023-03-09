#include "json_i_node_parser.h"

#include "core_i_node.h"
#include "crude_json.h"
#include "imgui_node_editor.h"
#include "json_container_serializer.h"
#include "json_id_serializer.h"
#include "json_optional_serializer.h"

namespace esc::json {
///
auto INodeParser::ParseFromJson(const crude_json::value& json) const
    -> std::unique_ptr<core::INode> {
  auto parsed_args = core::INode::ConstructorArgs{
      .id = IdSerializer::ParseFromJson<ne::NodeId>(json["id"]),
      .family_id =
          IdSerializer::ParseFromJson<core::FamilyId>(json["family_id"]),
      .input_pin_id = OptionalSerializer::ParseFromJson<ne::PinId>(
          json, "input_pin_id", &IdSerializer::ParseFromJson<ne::PinId>),
      .output_pin_ids = ContainerSerializer::ParseFromJson<ne::PinId>(
          json, "output_pin_ids", &IdSerializer::ParseFromJson<ne::PinId>),
      .pos = {static_cast<float>(json["pos_x"].get<crude_json::number>()),
              static_cast<float>(json["pos_y"].get<crude_json::number>())}};

  return ParseFromJson(std::move(parsed_args), json["data"]);
}
}  // namespace esc::json