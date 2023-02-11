#ifndef VH_JSON_ID_SERIALIZER_H_
#define VH_JSON_ID_SERIALIZER_H_

#include "crude_json.h"

namespace esc::json {
// ---
struct IdSerializer {
  // ---
  template <typename Id>
  static auto ParseFromJson [[nodiscard]] (const crude_json::value &json) {
    return Id{static_cast<uint64_t>(json.get<crude_json::number>())};
  }

  // ---
  template <typename Id>
  static auto WriteToJson [[nodiscard]] (Id id) {
    return crude_json::value{static_cast<crude_json::number>(id.Get())};
  }
};
}  // namespace esc::json

#endif  // VH_JSON_ID_SERIALIZER_H_
