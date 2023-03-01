#include "coreui_node.h"

#include "cpp_assert.h"

namespace esc::coreui {
///
auto Node::FindPin(const Node& node, ne::PinId pin_id) -> const Pin& {
  for (const auto& pins : {&node.input_pins, &node.output_pins}) {
    const auto pin =
        std::find_if(pins->begin(), pins->end(), [pin_id](const auto& pin) {
          return pin.icon.has_value() && (pin.icon->GetData().id == pin_id);
        });

    if (pin != pins->end()) {
      return *pin;
    }
  }

  Expects(false);
}
}  // namespace esc::coreui