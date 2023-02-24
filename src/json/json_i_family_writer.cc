/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#include "json_i_family_writer.h"

#include "json_id_serializer.h"

namespace esc::json {
auto IFamilyWriter::WriteToJson(const core::IFamily& family) const
    -> crude_json::value {
  auto json = crude_json::value{};
  json["type"] = GetTypeName();
  json["id"] = IdSerializer::WriteToJson(family.GetId());
  json["data"] = WriteToJson();
  return json;
}
}  // namespace esc::json