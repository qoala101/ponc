/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "json_tag_serializer.h"

#include <crude_json.h>
#include <imgui_node_editor.h>

#include <memory>

#include "core_tag.h"
#include "json_color_serializer.h"

namespace vh::ponc::json {
///
auto TagSerializer::ParseFromJson(const crude_json::value& json) -> core::Tag {
  return {.name = json["name"].get<crude_json::string>(),
          .color = ColorSerializer::ParseFromJson(json["color"])};
}

///
auto TagSerializer::WriteToJson(const core::Tag& tag) -> crude_json::value {
  auto json = crude_json::value{};
  json["name"] = tag.name;
  json["color"] = ColorSerializer::WriteToJson(tag.color);
  return json;
}

///
auto SharedTagSerializer::ParseFromJson(const crude_json::value& json)
    -> std::shared_ptr<core::Tag> {
  return std::make_shared<core::Tag>(TagSerializer::ParseFromJson(json));
}

///
auto SharedTagSerializer::WriteToJson(const std::shared_ptr<core::Tag>& tag)
    -> crude_json::value {
  return TagSerializer::WriteToJson(*tag);
}
}  // namespace vh::ponc::json