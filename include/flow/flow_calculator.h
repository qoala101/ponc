#ifndef VH_FLOW_CALCULATOR_H_
#define VH_FLOW_CALCULATOR_H_

#include <imgui_node_editor.h>

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "core_i_family.h"
#include "core_i_node.h"
#include "core_link.h"
#include "flow_node_flow.h"
#include "flow_tree.h"

namespace esc::core {
struct CoreState;
}

namespace esc::flow {
// ---
class FlowCalculator {
 public:
  // ---
  void OnFrame(const core::CoreState &core_state);
  // ---
  auto GetFlowTree() const -> const Tree &;
  // ---
  auto GetCalculatedFlow(const core::INode &node) const -> const NodeFlow &;

 private:
  // ---
  void RebuildFlowTree(
      const std::vector<std::shared_ptr<core::IFamily>> &families,
      const std::vector<core::Link> &links);
  // ---
  void RecalculateFlowValues();
  // ---
  void CalculateFlowValuesForNode(const TreeNode &node,
                                  float input_from_parent = {});

  // ---
  Tree flow_tree_{};
  // ---
  std::unordered_map<uintptr_t, NodeFlow> node_flows_{};
};
}  // namespace esc::flow

#endif  // VH_FLOW_CALCULATOR_H_
