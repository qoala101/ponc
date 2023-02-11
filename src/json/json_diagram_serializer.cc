#include "json_diagram_serializer.h"

#include <memory>

#include "core_diagram.h"
#include "core_i_node.h"
#include "crude_json.h"
#include "json_node_factory_serializer.h"
#include "json_node_serializer.h"

namespace esc::json {
namespace {
auto ParseNodeFactories
    [[nodiscard]] (const crude_json::value& json,
                   const std::vector<std::unique_ptr<INodeFactoryParser>>&
                       node_factory_parsers) {
  const auto node_factories_size =
      json["node_factories_size"].get<crude_json::number>();
  const auto& node_factories_json = json["node_factories"];

  auto parsed_node_factories =
      std::vector<std::shared_ptr<core::INodeFactory>>{};

  for (auto i = 0; i < node_factories_size; ++i) {
    const auto& node_factory_json = node_factories_json[i];

    for (const auto& parser : node_factory_parsers) {
      auto parsed_node_factory = parser->TryToParseFromJson(node_factory_json);

      if (!parsed_node_factory.has_value()) {
        continue;
      }

      parsed_node_factories.emplace_back(std::move(*parsed_node_factory));
    }
  }

  return parsed_node_factories;
}

auto ParseNodes
    [[nodiscard]] (const crude_json::value& json,
                   const std::vector<std::shared_ptr<core::INodeFactory>>&
                       parsed_node_factories) {
  const auto nodes_size = json["nodes_size"].get<crude_json::number>();
  const auto& nodes_json = json["nodes"];

  auto parsed_nodes = std::vector<std::shared_ptr<core::INode>>{};

  for (auto i = 0; i < nodes_size; ++i) {
    const auto& node_json = nodes_json[i];

    for (const auto& node_factory : parsed_node_factories) {
      auto parsed_node =
          node_factory->CreateNodeParser()->TryToParseFromJson(node_json);

      if (!parsed_node.has_value()) {
        continue;
      }

      parsed_nodes.emplace_back(std::move(*parsed_node));
    }
  }

  return parsed_nodes;
}

void WriteNodeFactories(
    const std::vector<std::shared_ptr<core::INodeFactory>>& node_factories,
    crude_json::value& json) {
  json["node_factories_size"] =
      static_cast<crude_json::number>(node_factories.size());
  auto& node_factories_json = json["node_factories"];

  for (auto i = 0; i < static_cast<int>(node_factories.size()); ++i) {
    const auto& node_factory = node_factories[i];
    node_factories_json[i] =
        node_factory->CreateWriter()->WriteToJson(*node_factory);
  }
}

void WriteNodes(const std::vector<std::shared_ptr<core::INode>>& nodes,
                crude_json::value& json) {
  json["nodes_size"] = static_cast<crude_json::number>(nodes.size());
  auto& nodes_json = json["nodes"];

  for (auto i = 0; i < static_cast<int>(nodes.size()); ++i) {
    const auto& node = nodes[i];
    nodes_json[i] = node->CreateWriter()->WriteToJson(*node);
  }
}
}  // namespace

auto DiagramSerializer::ParseFromJson(
    const crude_json::value& json,
    const std::vector<std::unique_ptr<INodeFactoryParser>>&
        node_factory_parsers) -> core::Diagram {
  auto parsed_node_factories = ParseNodeFactories(json, node_factory_parsers);
  auto parsed_nodes = ParseNodes(json, parsed_node_factories);
  return core::Diagram{
      std::move(parsed_node_factories), std::move(parsed_nodes), {}};
}

auto DiagramSerializer::WriteToJson(const core::Diagram& diagram)
    -> crude_json::value {
  auto json = crude_json::value{};
  WriteNodeFactories(diagram.GetNodeFactories(), json);
  WriteNodes(diagram.GetNodes(), json);
  return json;
}
}  // namespace esc::json