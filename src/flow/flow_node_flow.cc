/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "flow_node_flow.h"

#include <imgui_node_editor.h>

#include <algorithm>

#include "cpp_assert.h"

namespace vh::ponc::flow {
///
auto NodeFlow::GetPinFlow(const NodeFlow &flow, ne::PinId pin_id) -> float {
  const auto pin_id_value = pin_id.Get();

  if (flow.input_pin_flow.has_value() &&
      (flow.input_pin_flow->first == pin_id_value)) {
    return flow.input_pin_flow->second;
  }

  const auto pin_flow =
      std::find_if(flow.output_pin_flows.cbegin(), flow.output_pin_flows.cend(),
                   [pin_id_value](const auto &output_pin) {
                     return output_pin.first == pin_id_value;
                   });

  Expects(pin_flow != flow.output_pin_flows.cend());
  return pin_flow->second;
}

///
auto operator+(NodeFlow &left, const NodeFlow &right) -> NodeFlow & {
  if (left.input_pin_flow.has_value()) {
    auto &left_input = left.input_pin_flow->second;

    Expects(right.input_pin_flow.has_value());
    const auto right_input = right.input_pin_flow->second;

    left_input += right_input;
  }

  for (auto &[left_output_pin, left_output] : left.output_pin_flows) {
    Expects(right.output_pin_flows.contains(left_output_pin));
    const auto right_output = right.output_pin_flows.at(left_output_pin);

    left_output += right_output;
  }

  return left;
}

///
auto operator+(NodeFlow &left, float right) -> NodeFlow & {
  if (left.input_pin_flow.has_value()) {
    auto &left_input = left.input_pin_flow->second;
    left_input += right;
  }

  for (auto &[left_output_pin, left_output] : left.output_pin_flows) {
    left_output += right;
  }

  return left;
}

///
void operator+=(NodeFlow &left, const NodeFlow &right) { left = left + right; }

///
void operator+=(NodeFlow &left, float right) { left = left + right; }
}  // namespace vh::ponc::flow