/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
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
                            const crude_json::string& items_name,
                            const auto& item_parser) {
    const auto items_size =
        json[items_name + "_size"].get<crude_json::number>();
    const auto& items_json = json[items_name];

    auto parsed_items = std::vector<Item>{};
    parsed_items.reserve(items_size);

    for (auto i = 0; i < items_size; ++i) {
      parsed_items.emplace_back(item_parser(items_json[i]));
    }

    return parsed_items;
  }

  ///
  template <typename Item>
  static auto WriteToJson(crude_json::value& json,
                          const std::vector<Item>& items,
                          const crude_json::string& items_name,
                          const auto& item_writer) {
    json[items_name + "_size"] = static_cast<crude_json::number>(items.size());
    auto& items_json = json[items_name];

    for (auto i = 0; i < static_cast<int>(items.size()); ++i) {
      items_json[i] = item_writer(items[i]);
    }
  }
};
}  // namespace vh::ponc::json

#endif  // VH_PONC_JSON_CONTAINER_SERIALIZER_H_
