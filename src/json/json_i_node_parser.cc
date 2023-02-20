#include "json_i_node_parser.h"

#include "core_i_node.h"
#include "crude_json.h"
#include "imgui_node_editor.h"
#include "json_id_serializer.h"

namespace esc::json {
namespace {
auto ParsePinIds [[nodiscard]] (const crude_json::value& json) {
  const auto pin_ids_size = json["pin_ids_size"].get<crude_json::number>();
  const auto& pin_ids_json = json["pin_ids"];

  auto parsed_pin_ids = std::vector<ne::PinId>{};

  for (auto i = 0; i < pin_ids_size; ++i) {
    parsed_pin_ids.emplace_back(
        IdSerializer::ParseFromJson<ne::PinId>(pin_ids_json[i]));
  }

  return parsed_pin_ids;
}
}  // namespace

auto INodeParser::ParseFromJson(const crude_json::value& json) const
    -> std::shared_ptr<core::INode> {
  const auto parsed_id = IdSerializer::ParseFromJson<ne::NodeId>(json["id"]);
  auto parsed_pin_ids = ParsePinIds(json);
  auto parsed_node =
      ParseFromJson(parsed_id, std::move(parsed_pin_ids), json["data"]);

  const auto parsed_position =
      ImVec2{static_cast<float>(json["pos_x"].get<crude_json::number>()),
             static_cast<float>(json["pos_y"].get<crude_json::number>())};
  parsed_node->SetPosition(parsed_position);

  return parsed_node;
}
}  // namespace esc::json