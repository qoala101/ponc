#include "json_i_node_factory_writer.h"

#include "json_i_node_writer.h"

namespace esc::json {
namespace {
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

auto INodeFactoryWriter::WriteToJson(
    const core::INodeFactory& node_factory) const -> crude_json::value {
  auto json = crude_json::value{};

  json["type"] = GetTypeName();
  json["data"] = WriteToJson();
  WriteNodes(node_factory.GetNodes(), json);

  return json;
}
}  // namespace esc::json