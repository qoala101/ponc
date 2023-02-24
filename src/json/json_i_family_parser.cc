/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#include "json_i_family_parser.h"

#include "core_family_id.h"
#include "json_id_serializer.h"

namespace esc::json {
auto IFamilyParser::TryToParseFromJson(const crude_json::value& json) const
    -> std::optional<std::shared_ptr<core::IFamily>> {
  if (json["type"].get<crude_json::string>() != GetTypeName()) {
    return std::nullopt;
  }

  const auto id = IdSerializer::ParseFromJson<core::FamilyId>(json["id"]);
  return ParseFromJson(id, json["data"]);
}
}  // namespace esc::json