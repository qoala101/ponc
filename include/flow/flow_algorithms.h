#ifndef VH_FLOW_BUILD_FLOW_TREE_H_
#define VH_FLOW_BUILD_FLOW_TREE_H_

#include "core_diagram.h"
#include "flow_tree.h"

namespace esc::flow {
///
auto BuildFlowTree(const core::Diagram &diagram) -> FlowTree;
///
auto CalculateNodeFlows(const FlowTree &flow_tree)
    -> std::unordered_map<uintptr_t, NodeFlow>;
}  // namespace esc::flow

#endif  // VH_FLOW_BUILD_FLOW_TREE_H_
