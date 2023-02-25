#ifndef VH_JSON_CONTAINER_SERIALIZER_H_
#define VH_JSON_CONTAINER_SERIALIZER_H_

#include "crude_json.h"

namespace esc::json {
struct ContainerSerializer {
  template <typename Item>
  static auto ParseFromJson(const crude_json::value& json,
                            const std::string& items_name,
                            const auto& item_parser) {
    const auto items_size =
        json[items_name + "_size"].get<crude_json::number>();
    const auto& items_json = json[items_name];

    auto parsed_items = std::vector<Item>{};

    for (auto i = 0; i < items_size; ++i) {
      parsed_items.emplace_back(item_parser(items_json[i]));
    }

    return parsed_items;
  }

  template <typename Item>
  static auto WriteToJson(crude_json::value& json,
                          const std::vector<Item>& items,
                          const std::string& items_name,
                          const auto& item_writer) {
    const auto items_size = items.size();

    json[items_name + "_size"] = static_cast<crude_json::number>(items_size);
    auto& links_json = json[items_name];

    for (auto i = 0; i < static_cast<int>(items_size); ++i) {
      links_json[i] = item_writer(items[i]);
    }
  }
};
}  // namespace esc::json

#endif  // VH_JSON_CONTAINER_SERIALIZER_H_
