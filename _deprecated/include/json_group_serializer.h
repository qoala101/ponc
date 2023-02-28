#ifndef VH_JSON_GROUP_SERIALIZER_H_
#define VH_JSON_GROUP_SERIALIZER_H_

#include "core_group.h"
#include "crude_json.h"

namespace esc::json {
struct GroupSerializer {
  static auto ParseFromJson(const crude_json::value &json) -> core::Group;
  static auto WriteToJson(const core::Group &group) -> crude_json::value;
};
}  // namespace esc::json

#endif  // VH_JSON_GROUP_SERIALIZER_H_
