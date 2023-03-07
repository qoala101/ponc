#ifndef VH_DRAW_ID_LABEL_H_
#define VH_DRAW_ID_LABEL_H_

#include <concepts>
#include <string>
#include <string_view>

#include "core_concepts.h"

namespace esc::draw {
///
template <std::integral T>
auto IdLabel(std::string_view label, T id) {
  return label.data() + std::string{"##"} + std::to_string(id);
}

///
template <core::Id T>
auto IdLabel(std::string_view label, T id) {
  return IdLabel(label, id.Get());
}
}  // namespace esc::draw

#endif  // VH_DRAW_ID_LABEL_H_
