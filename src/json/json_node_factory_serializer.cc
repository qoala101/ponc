#include "json_node_factory_serializer.h"

namespace esc::json {
auto INodeFactoryParser::TryToParseFromJson(const crude_json::value& json) const
    -> std::optional<std::shared_ptr<core::INodeFactory>> {
  if (json["type"].get<crude_json::string>() != GetTypeName()) {
    return std::nullopt;
  }

  return ParseFromJson(json["data"]);
}

auto INodeFactoryWriter::WriteToJson(const core::INodeFactory& /*unused*/) const
    -> crude_json::value {
  auto json = crude_json::value{};

  json["type"] = GetTypeName();
  json["data"] = WriteToJson();

  return json;
}
}  // namespace esc::json