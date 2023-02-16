#include "core_flow.h"

#include <algorithm>
#include <ranges>

#include "cpp_assert.h"

namespace esc::core {
// ---
auto operator+(Flow &left, const Flow &right) -> Flow & {
  if (left.input_pin_flow.has_value()) {
    auto &left_input = left.input_pin_flow->second;

    Expects(right.input_pin_flow.has_value());
    const auto right_input = right.input_pin_flow->second;

    left_input += right_input;
  }

  for (auto &[left_output_pin, left_output] : left.output_pin_flows) {
    const auto right_output = right.output_pin_flows.at(left_output_pin);
    left_output += right_output;
  }

  return left;
}

// ---
auto operator+(Flow &left, float right) -> Flow & {
  if (left.input_pin_flow.has_value()) {
    auto &left_input = left.input_pin_flow->second;
    left_input += right;
  }

  for (auto &[left_output_pin, left_output] : left.output_pin_flows) {
    left_output += right;
  }

  return left;
}

// ---
void operator+=(Flow &left, const Flow &right) { left = left + right; }

// ---
void operator+=(Flow &left, float right) { left = left + right; }

// ---
auto GetPinFlow(const Flow &flow, ne::PinId pin_id) -> float {
  const auto pin_id_as_number = pin_id.Get();

  if (flow.input_pin_flow.has_value() &&
      (flow.input_pin_flow->first == pin_id_as_number)) {
    return flow.input_pin_flow->second;
  }

  return std::ranges::find_if(flow.output_pin_flows,
                              [pin_id_as_number](const auto &output_pin) {
                                return output_pin.first == pin_id_as_number;
                              })
      ->second;
}
}  // namespace esc::core