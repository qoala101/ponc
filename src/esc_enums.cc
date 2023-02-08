#include "esc_enums.h"

#include "esc_cpp.h"
// vh: norm
auto GetIconColor(PinType pin_type) -> ImColor {
  switch (pin_type) {
    case PinType::Flow:
      return {255, 255, 255};
    default:
      cpp::Expects(false);
  }
}
// vh: norm
auto GetIconType(PinType pin_type) -> IconType {
  switch (pin_type) {
    case PinType::Flow:
      return IconType::Flow;
    default:
      cpp::Expects(false);
  }
}