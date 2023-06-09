/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_JSON_TAG_SERIALIZER_H_
#define VH_PONC_JSON_TAG_SERIALIZER_H_

#include <crude_json.h>

#include <memory>

#include "core_tag.h"
#include "cpp_static_api.h"

namespace vh::ponc::json {
///
struct TagSerializer : public cpp::StaticApi {
  ///
  static auto ParseFromJson(const crude_json::value &json) -> core::Tag;
  ///
  static auto WriteToJson(const core::Tag &tag) -> crude_json::value;
};

///
struct SharedTagSerializer : public cpp::StaticApi {
  ///
  static auto ParseFromJson(const crude_json::value &json)
      -> std::shared_ptr<core::Tag>;
  ///
  static auto WriteToJson(const std::shared_ptr<core::Tag> &tag)
      -> crude_json::value;
};
}  // namespace vh::ponc::json

#endif  // VH_PONC_JSON_TAG_SERIALIZER_H_
