#include "flow_node_flow.h"

#include <algorithm>
#include <iostream>
#include <ranges>

#include "cpp_assert.h"
#include "flow_tree.h"

namespace esc::flow {
namespace {
// NOLINTNEXTLINE(*-no-recursion)
void CalculateNodeFlow(std::unordered_map<uintptr_t, NodeFlow> &node_flows_,
                       const TreeNode &node, float input_from_parent = 0.F) {
  const auto node_id = node.node->GetId();
  auto calculated_flow = node_flows_.find(node_id.Get());

  const auto initial_flow = node.node->GetInitialFlow();

  if (calculated_flow == node_flows_.end()) {
    calculated_flow = node_flows_.emplace(node_id, initial_flow).first;
  } else {
    calculated_flow->second += initial_flow;
  }

  calculated_flow->second += input_from_parent;

  for (const auto &[child_pin, child_node] : node.child_nodes) {
    const auto added_flow =
        calculated_flow->second.output_pin_flows.at(child_pin);

    CalculateNodeFlow(node_flows_, child_node, added_flow);
  }
}
}  // namespace

auto operator+(NodeFlow &left, const NodeFlow &right) -> NodeFlow & {
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

void operator+=(NodeFlow &left, const NodeFlow &right) { left = left + right; }

void operator+=(NodeFlow &left, float right) { left = left + right; }

auto GetPinFlow(const NodeFlow &flow, ne::PinId pin_id) -> float {
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

auto CalculateNodeFlows(const Tree &flow_tree)
    -> std::unordered_map<uintptr_t, NodeFlow> {
  auto node_flows = std::unordered_map<uintptr_t, NodeFlow>{};

  for (const auto &node : flow_tree.root_nodes) {
    CalculateNodeFlow(node_flows, node);
  }

  return node_flows;
}
}  // namespace esc::flow