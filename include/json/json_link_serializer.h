#ifndef VH_JSON_LINK_SERIALIZER_H_
#define VH_JSON_LINK_SERIALIZER_H_

#include "core_link.h"
#include "crude_json.h"

namespace esc::json {
struct LinkSerializer {
  static auto ParseFromJson(const crude_json::value &json) -> core::Link;
  static auto WriteToJson(const core::Link &link) -> crude_json::value;
};
}  // namespace esc::json

#endif  // VH_JSON_LINK_SERIALIZER_H_
