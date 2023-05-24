/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_JSON_ID_SERIALIZER_H_
#define VH_PONC_JSON_ID_SERIALIZER_H_

#include <crude_json.h>

#include "core_concepts.h"
#include "core_id_value.h"
#include "cpp_static_api.h"

namespace vh::ponc::json {
///
struct IdSerializer : public cpp::StaticApi {
  ///
  template <core::Id Id>
  static auto ParseFromJson(const crude_json::value &json) {
    return Id{
        static_cast<core::UnspecifiedIdValue>(json.get<crude_json::number>())};
  }

  ///
  template <core::Id Id>
  static auto WriteToJson(Id id) {
    return crude_json::value{static_cast<crude_json::number>(id.Get())};
  }
};
}  // namespace vh::ponc::json

#endif  // VH_PONC_JSON_ID_SERIALIZER_H_
