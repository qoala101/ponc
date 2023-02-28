#include "coreui_i_pin_traits.h"

#include <optional>

namespace esc::coreui {
///
auto IPinTraits::GetLabel() const -> std::string { return {}; }

///
auto IPinTraits::GetValue() -> std::variant<std::monostate, float, float *> {
  return {};
}
}  // namespace esc::coreui