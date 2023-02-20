/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#include "json_group_serializer.h"

#include "core_group.h"
#include "crude_json.h"

namespace esc::json {
// ---
auto GroupSerializer::ParseFromJson(const crude_json::value& json)
    -> core::Group {
  return core::Group{};
}

// ---
auto GroupSerializer::WriteToJson(const core::Group& group)
    -> crude_json::value {
  auto json = crude_json::value{};
  return json;
}
}  // namespace esc::json