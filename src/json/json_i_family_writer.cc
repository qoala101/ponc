#include "json_i_family_writer.h"

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

auto IFamilyWriter::WriteToJson(const core::IFamily& family) const
    -> crude_json::value {
  auto json = crude_json::value{};

  json["type"] = GetTypeName();
  json["data"] = WriteToJson();
  WriteNodes(family.GetNodes(), json);

  return json;
}
}  // namespace esc::json