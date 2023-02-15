#ifndef VH_CORE_FLOW_CALCULATOR_H_
#define VH_CORE_FLOW_CALCULATOR_H_

#include <imgui_node_editor.h>

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "core_flow.h"
#include "core_i_family.h"
#include "core_i_node.h"
#include "core_link.h"
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
  auto GetCalculatedFlow(const INode &node) const -> const Flow &;

 private:  
  // ---
  void RebuildFlowTree(const std::vector<std::shared_ptr<IFamily>> &families,
                       const std::vector<Link> &links);
  // ---
  void RecalculateFlowValues();
  // ---
  void CalculateFlowValuesForNode(const TreeNode &node,
                                  float input_from_parent = {});

  // ---
  Tree flow_tree_{};
  // ---
  std::unordered_map<uintptr_t, Flow> node_flows_{};
};
}  // namespace core
}  // namespace esc

#endif  // VH_CORE_FLOW_CALCULATOR_H_
