#include "esc_enums.h"

#include "esc_cpp.h"
// vh: norm
auto GetIconColor(PinType pin_type) -> ImColor {
  switch (pin_type) {
    case PinType::Flow:
      return {255, 255, 255};
    case PinType::Bool:
      return {220, 48, 48};
    case PinType::Int:
      return {68, 201, 156};
    case PinType::Float:
      return {147, 226, 74};
    case PinType::String:
      return {124, 21, 153};
    case PinType::Object:
      return {51, 150, 215};
    case PinType::Function:
      return {218, 0, 183};
    case PinType::Delegate:
      return {255, 48, 48};
    default:
      cpp::Expects(false);
  }
}
// vh: norm
auto GetIconType(PinType pin_type) -> IconType {
  switch (pin_type) {
    case PinType::Flow:
      return IconType::Flow;
    case PinType::Bool:
    case PinType::Int:
    case PinType::Float:
    case PinType::String:
    case PinType::Object:
    case PinType::Function:
      return IconType::Circle;
    case PinType::Delegate:
      return IconType::Square;
    default:
      cpp::Expects(false);
  }
}