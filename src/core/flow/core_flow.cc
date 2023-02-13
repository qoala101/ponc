#include "core_flow.h"

#include "cpp_assert.h"

namespace esc::core {
// ---
auto operator+(Flow &left, const Flow &right) -> Flow & {
  if (left.input_pin_flow.has_value()) {
    auto &left_input = left.input_pin_flow->second;
    
    cpp::Expects(right.input_pin_flow.has_value());
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
}  // namespace esc::core