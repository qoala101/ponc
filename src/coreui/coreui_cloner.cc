/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "coreui_cloner.h"

#include <algorithm>

#include "json_diagram_serializer.h"
#include "json_i_node_parser.h"
#include "json_i_node_writer.h"

namespace vh::ponc::coreui {
///
auto Cloner::Clone(const core::Diagram& diagram,
                   const std::vector<std::unique_ptr<core::IFamily>>& families)
    -> core::Diagram {
  const auto json = json::DiagramSerializer::WriteToJson(diagram);
  return json::DiagramSerializer::ParseFromJson(json, families);
}

///
auto Cloner::Clone(const core::INode& node, const core::IFamily& family)
    -> std::unique_ptr<core::INode> {
  const auto json = node.CreateWriter()->WriteToJson(node);
  return family.CreateNodeParser()->ParseFromJson(json);
}

///
void Cloner::RewireIds(const std::vector<core::IdPtr>& ids,
                       core::IdGenerator& id_generator) {
  if (ids.empty()) {
    return;
  }

  const auto next_id = id_generator.GetNextId();

  const auto [min_id, max_id] = std::minmax_element(
      ids.cbegin(), ids.cend(), [](const auto& left, const auto& right) {
        return GetValue(left) < GetValue(right);
      });
  const auto min_id_value = core::GetValue(*min_id);
  const auto max_id_value = core::GetValue(*max_id);

  for (const auto& id : ids) {
    core::SetValue(id, core::GetValue(id) + next_id - min_id_value);
  }

  id_generator = core::IdGenerator{next_id - min_id_value + max_id_value + 1};
}
}  // namespace vh::ponc::coreui