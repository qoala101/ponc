#include "core_link.h"

namespace esc::core {
auto HasPin(const Link &link, ne::PinId pin_id) -> bool {
  return (pin_id == link.start_pin_id) || (pin_id == link.end_pin_id);
}
}  // namespace esc::core