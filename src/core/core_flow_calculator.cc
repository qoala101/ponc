#include "core_flow_calculator.h"

#include <iostream>

#include "core_i_node.h"
#include "esc_state.h"

namespace esc::core {
void FlowCalculator::OnFrame(const State &state) {
  std::cout << "calc on frame: " << pin_values_.size() << "\n";

  pin_values_.clear();

  for (const auto &family : state.app_.GetDiagram().GetFamilies()) {
    for (const auto &node : family->GetNodes()) {
      const auto flow_values = node->GetFlowValues();

      if (flow_values.parent_value.has_value()) {
        pin_values_[flow_values.parent_value->id.Get()] =
            flow_values.parent_value->value;
      }

      for (const auto &child_value : flow_values.child_values) {
        pin_values_[child_value.id.Get()] = child_value.value;
      }
    }
  }

  std::cout << pin_values_.size() << "\n";
}

auto FlowCalculator::GetCalculatedFlowValues(const INode &node) const
    -> FlowValues {
  std::cout << "GetCalculatedFlowValues: " << pin_values_.size() << "\n";

  auto flow_values = node.GetFlowValues();

  if (flow_values.parent_value.has_value()) {
    flow_values.parent_value->value =
        pin_values_.at(flow_values.parent_value->id.Get());
  }

  for (auto &child_pin_value : flow_values.child_values) {
    child_pin_value.value = pin_values_.at(child_pin_value.id.Get());
  }

  return flow_values;
}
}  // namespace esc::core