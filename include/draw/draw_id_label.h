#ifndef VH_DRAW_ID_LABEL_H_
#define VH_DRAW_ID_LABEL_H_

#include <concepts>
#include <string>
#include <string_view>

#include "core_concepts.h"

namespace esc::draw {
///
template <std::integral T>
auto IdLabel(T id, std::string_view label = {}) {
  auto id_label = std::string{"##"} + std::to_string(id);

  if (label.empty()) {
    return id_label;
  }

  return label.data() + std::move(id_label);
}

///
template <core::Id T>
auto IdLabel(T id, std::string_view label = {}) {
  return IdLabel(id.Get(), label);
}
}  // namespace esc::draw

#endif  // VH_DRAW_ID_LABEL_H_
