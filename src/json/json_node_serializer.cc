#include "json_node_serializer.h"

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

void WritePinIds(const std::vector<ne::PinId>& pin_ids,
                 crude_json::value& json) {
  json["pin_ids_size"] = static_cast<crude_json::number>(pin_ids.size());
  auto& pin_ids_json = json["pin_ids"];

  for (auto i = 0; i < static_cast<int>(pin_ids.size()); ++i) {
    pin_ids_json[i] = IdSerializer::WriteToJson(pin_ids[i]);
  }
}
}  // namespace

auto INodeParser::TryToParseFromJson(const crude_json::value& json) const
    -> std::optional<std::shared_ptr<core::INode>> {
  if (json["type"].get<crude_json::string>() != GetTypeName()) {
    return std::nullopt;
  }

  const auto parsed_node_id =
      IdSerializer::ParseFromJson<ne::NodeId>(json["id"]);
  auto parsed_pin_ids = ParsePinIds(json);

  return ParseFromJson(parsed_node_id, std::move(parsed_pin_ids), json["data"]);
}

auto INodeWriter::WriteToJson(const core::INode& node) const
    -> crude_json::value {
  auto json = crude_json::value{};

  json["type"] = GetTypeName();
  json["id"] = IdSerializer::WriteToJson(node.GetId());
  WritePinIds(node.GetPinIds(), json);

  const auto pos = node.GetPosition();
  json["pos_x"] = pos.x;
  json["pos_y"] = pos.y;
  json["data"] = WriteToJson();

  return json;
}
}  // namespace esc::json