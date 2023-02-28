#include "json_diagram_serializer.h"

#include <memory>

#include "core_diagram.h"
#include "core_family_id.h"
#include "core_i_node.h"
#include "core_link.h"
#include "cpp_assert.h"
#include "crude_json.h"
#include "json_container_serializer.h"
#include "json_i_node_parser.h"
#include "json_i_node_writer.h"
#include "json_id_serializer.h"
#include "json_link_serializer.h"

namespace esc::json {
namespace {
auto FindFamily(const std::vector<std::shared_ptr<core::IFamily>>& families,
                core::FamilyId family_id) -> auto& {
  const auto family = std::ranges::find_if(
      families,
      [family_id](const auto& family) { return family->GetId() == family_id; });
  Expects(family != families.end());
  return *family;
}

auto ParseNode(const crude_json::value& json,
               const std::vector<std::shared_ptr<core::IFamily>>& families) {
  const auto family_id =
      IdSerializer::ParseFromJson<core::FamilyId>(json["family_id"]);
  const auto& family = FindFamily(families, family_id);
  return family->CreateNodeParser()->ParseFromJson(json);
}
}  // namespace

auto DiagramSerializer::ParseFromJson(
    const crude_json::value& json,
    const std::vector<std::shared_ptr<core::IFamily>>& families)
    -> core::Diagram {
  auto nodes = ContainerSerializer::ParseFromJson<std::shared_ptr<core::INode>>(
      json, "nodes",
      [&families](const auto& json) { return ParseNode(json, families); });
  auto links = ContainerSerializer::ParseFromJson<core::Link>(
      json, "links", &LinkSerializer::ParseFromJson);
  return core::Diagram{std::move(nodes), std::move(links)};
}

auto DiagramSerializer::WriteToJson(const core::Diagram& diagram)
    -> crude_json::value {
  auto json = crude_json::value{};
  ContainerSerializer::WriteToJson(
      json, diagram.GetNodes(), "nodes", [](const auto& node) {
        return node->CreateWriter()->WriteToJson(*node);
      });
  ContainerSerializer::WriteToJson(json, diagram.GetLinks(), "links",
                                   &LinkSerializer::WriteToJson);
  return json;
}
}  // namespace esc::json