#ifndef VH_JSON_OPTIONAL_SERIALIZER_H_
#define VH_JSON_OPTIONAL_SERIALIZER_H_

#include "crude_json.h"

namespace esc::json {
///
struct OptionalSerializer {
  ///
  template <typename Item>
  static auto ParseFromJson(const crude_json::value& json,
                            const std::string& item_name,
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
                          const std::string& item_name,
                          const auto& item_writer) {
    if (!item.has_value()) {
      return;
    }

    json[item_name] = item_writer(*item);
  }
};
}  // namespace esc::json

#endif  // VH_JSON_OPTIONAL_SERIALIZER_H_
