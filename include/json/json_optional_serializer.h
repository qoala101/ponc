#ifndef VH_PONC_JSON_OPTIONAL_SERIALIZER_H_
#define VH_PONC_JSON_OPTIONAL_SERIALIZER_H_

#include "cpp_no_instances.h"
#include "crude_json.h"

namespace vh::ponc::json {
///
struct OptionalSerializer : public cpp::NoInstances {
  ///
  template <typename Item>
  static auto ParseFromJson(const crude_json::value& json,
                            const crude_json::string& item_name,
                            const auto& item_parser) -> std::optional<Item> {
    if (!json.contains(item_name)) {
      return std::nullopt;
    }

    return item_parser(json[item_name]);
  }

  ///
  template <typename Item>
  static auto WriteToJson(crude_json::value& json,
                          const std::optional<Item>& item,
                          const crude_json::string& item_name,
                          const auto& item_writer) {
    if (!item.has_value()) {
      return;
    }

    json[item_name] = item_writer(*item);
  }
};
}  // namespace vh::ponc::json

#endif  // VH_PONC_JSON_OPTIONAL_SERIALIZER_H_
