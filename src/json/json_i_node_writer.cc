/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "json_i_node_writer.h"

#include <crude_json.h>
#include <imgui.h>
#include <imgui_node_editor.h>

#include <algorithm>
#include <memory>

#include "core_i_node.h"
#include "json_container_serializer.h"
#include "json_id_serializer.h"
#include "json_optional_serializer.h"

namespace vh::ponc::json {
///
auto INodeWriter::WriteToJson(const core::INode& node) const
    -> crude_json::value {
  auto json = crude_json::value{};
  json["id"] = IdSerializer::WriteToJson(node.GetId());
  json["family_id"] = IdSerializer::WriteToJson(node.GetFamilyId());

  OptionalSerializer::WriteToJson(json, node.GetInputPinId(), "input_pin_id",
                                  &IdSerializer::WriteToJson<ne::PinId>);

  json["output_pin_ids"] = ContainerSerializer::WriteToJson(
      node.GetOutputPinIds(), &IdSerializer::WriteToJson<ne::PinId>);

  const auto pos = node.GetPos();
  json["pos"] = crude_json::array{pos.x, pos.y};

  json["data"] = WriteToJson();
  return json;
}

///
auto INodeWriter::WriteToJson() const -> crude_json::value { return {}; }
}  // namespace vh::ponc::json