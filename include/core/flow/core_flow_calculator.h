#ifndef VH_CORE_FLOW_CALCULATOR_H_
#define VH_CORE_FLOW_CALCULATOR_H_

#include <imgui_node_editor.h>

#include <map>
#include <unordered_map>
#include <vector>

#include "core_i_node.h"
#include "core_tree.h"

namespace esc {
class State;

namespace core {
// ---
class FlowCalculator {
 public:
  // ---
  void OnFrame(const State &state);
  // ---
  auto GetFlowTree() const -> const Tree &;
  // ---
  auto GetCalculatedFlow(const INode &node) const -> NodeFlow;

 private:
  void RebuildFlowTree(const State &state);
  void CalculateFlowValues(const State &state);

  Tree flow_tree_{};
  std::unordered_map<uintptr_t, float> pin_values_{};
};
}  // namespace core
}  // namespace esc

#endif  // VH_CORE_FLOW_CALCULATOR_H_
