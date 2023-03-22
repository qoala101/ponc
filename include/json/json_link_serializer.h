#ifndef VH_PONC_JSON_LINK_SERIALIZER_H_
#define VH_PONC_JSON_LINK_SERIALIZER_H_

#include "core_link.h"
#include "cpp_no_instances.h"
#include "crude_json.h"

namespace vh::ponc::json {
///
struct LinkSerializer : public cpp::NoInstances {
  ///
  static auto ParseFromJson(const crude_json::value &json) -> core::Link;
  ///
  static auto WriteToJson(const core::Link &link) -> crude_json::value;
};
}  // namespace vh::ponc::json

#endif  // VH_PONC_JSON_LINK_SERIALIZER_H_
