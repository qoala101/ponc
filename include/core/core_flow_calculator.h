#ifndef VH_CORE_FLOW_CALCULATOR_H_
#define VH_CORE_FLOW_CALCULATOR_H_

#include <imgui_node_editor.h>

#include <map>
#include <unordered_map>
#include <vector>

#include "core_i_node.h"

namespace esc {
class State;

namespace core {
class FlowCalculator {
 public:
  void OnFrame(const State &state);

  auto GetCalculatedFlowValues(const INode &node) const -> FlowValues;

 private:
  std::unordered_map<uintptr_t, float> pin_values_{};
};
}  // namespace core
}  // namespace esc

#endif  // VH_CORE_FLOW_CALCULATOR_H_
