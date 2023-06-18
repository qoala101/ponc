/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_JSON_CONTAINER_SERIALIZER_H_
#define VH_PONC_JSON_CONTAINER_SERIALIZER_H_

#include <crude_json.h>

#include "cpp_static_api.h"

namespace vh::ponc::json {
///
struct ContainerSerializer : public cpp::StaticApi {
  ///
  template <typename Item>
  static auto ParseFromJson(const crude_json::value& json,
                            const auto& item_parser) {
    const auto& items_json = json.get<crude_json::array>();

    auto parsed_items = std::vector<Item>{};
    parsed_items.reserve(items_json.size());

    std::transform(items_json.cbegin(), items_json.cend(),
                   std::back_inserter(parsed_items),
                   [&item_parser](const auto& item_json) {
                     return item_parser(item_json);
                   });

    return parsed_items;
  }

  ///
  template <typename Item>
  static auto WriteToJson(const std::vector<Item>& items,
                          const auto& item_writer) {
    auto items_json = crude_json::array{};
    items_json.reserve(items.size());

    std::transform(
        items.cbegin(), items.cend(), std::back_inserter(items_json),
        [&item_writer](const auto& item) { return item_writer(item); });

    return items_json;
  }
};
}  // namespace vh::ponc::json

#endif  // VH_PONC_JSON_CONTAINER_SERIALIZER_H_
