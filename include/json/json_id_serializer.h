#ifndef VH_PONC_JSON_ID_SERIALIZER_H_
#define VH_PONC_JSON_ID_SERIALIZER_H_

#include "cpp_no_instances.h"
#include "crude_json.h"

namespace vh::ponc::json {
///
struct IdSerializer : public cpp::NoInstances {
  ///
  template <typename Id>
  static auto ParseFromJson(const crude_json::value &json) {
    return Id{static_cast<uintptr_t>(json.get<crude_json::number>())};
  }

  ///
  template <typename Id>
  static auto WriteToJson(Id id) {
    return crude_json::value{static_cast<crude_json::number>(id.Get())};
  }
};
}  // namespace vh::ponc::json

#endif  // VH_PONC_JSON_ID_SERIALIZER_H_
