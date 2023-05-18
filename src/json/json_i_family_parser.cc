/**
 * PON Calculator https://github.com/qoala101/ponc
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "json_i_family_parser.h"

#include <memory>

#include "json_id_serializer.h"

namespace vh::ponc::json {
///
auto IFamilyParser::TryToParseFromJson(const crude_json::value& json) const
    -> std::optional<std::unique_ptr<core::IFamily>> {
  if (json["type"].get<crude_json::string>() != GetTypeName()) {
    return std::nullopt;
  }

  const auto id = IdSerializer::ParseFromJson<core::FamilyId>(json["id"]);
  return ParseFromJson(id, json["data"]);
}
}  // namespace vh::ponc::json