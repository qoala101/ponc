#include "json_i_node_factory_parser.h"

#include "json_i_node_parser.h"

namespace esc::json {
namespace {
auto ParseNodes [[nodiscard]] (const crude_json::value& json,
                               const INodeParser& node_parser) {
  const auto nodes_size = json["nodes_size"].get<crude_json::number>();
  const auto& nodes_json = json["nodes"];

  auto parsed_nodes = std::vector<std::shared_ptr<core::INode>>{};

  for (auto i = 0; i < nodes_size; ++i) {
    parsed_nodes.emplace_back(node_parser.ParseFromJson(nodes_json[i]));
  }

  return parsed_nodes;
}
}  // namespace

auto INodeFactoryParser::TryToParseFromJson(const crude_json::value& json) const
    -> std::optional<std::shared_ptr<core::INodeFactory>> {
  if (json["type"].get<crude_json::string>() != GetTypeName()) {
    return std::nullopt;
  }

  const auto& factory_data_json = json["data"];
  const auto node_parser =
      ParseFromJson({}, factory_data_json)->CreateNodeParser();

  auto parsed_nodes = ParseNodes(json, *node_parser);
  auto parsed_factory =
      ParseFromJson(std::move(parsed_nodes), factory_data_json);

  return parsed_factory;
}
}  // namespace esc::json