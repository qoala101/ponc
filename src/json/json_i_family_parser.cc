#include "json_i_family_parser.h"

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

auto IFamilyParser::TryToParseFromJson(const crude_json::value& json) const
    -> std::optional<std::shared_ptr<core::IFamily>> {
  if (json["type"].get<crude_json::string>() != GetTypeName()) {
    return std::nullopt;
  }

  const auto& family_data_json = json["data"];
  const auto node_parser =
      ParseFromJson({}, family_data_json)->CreateNodeParser();

  auto parsed_nodes = ParseNodes(json, *node_parser);
  auto parsed_family = ParseFromJson(std::move(parsed_nodes), family_data_json);

  return parsed_family;
}
}  // namespace esc::json