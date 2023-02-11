#include "json_node_serializer.h"

#include "core_i_node.h"

namespace esc::json {
auto INodeParser::TryToParseFromJson(const crude_json::value& json) const
    -> std::optional<std::shared_ptr<core::INode>> {
  if (json["type"].get<crude_json::string>() != GetTypeName()) {
    return std::nullopt;
  }

  return ParseFromJson(json["data"]);
}

auto INodeWriter::WriteToJson(const core::INode& node) const
    -> crude_json::value {
  auto json = crude_json::value{};
  const auto pos = node.GetPosition();

  json["pos_x"] = pos.x;
  json["pos_y"] = pos.y;
  json["type"] = GetTypeName();
  json["data"] = WriteToJson();

  return json;
}
}  // namespace esc::json