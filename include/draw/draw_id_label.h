/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_DRAW_ID_LABEL_H_
#define VH_DRAW_ID_LABEL_H_

#include <string>
#include <string_view>

namespace esc::draw {
// ---
template <typename Id>
auto IdLabel [[nodiscard]] (std::string_view label, Id id) {
  return label.data() + std::string{"##"} + std::to_string(id.Get());
}
}  // namespace esc::draw

#endif  // VH_DRAW_ID_LABEL_H_
