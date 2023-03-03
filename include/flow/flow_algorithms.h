#ifndef VH_FLOW_BUILD_FLOW_TREE_H_
#define VH_FLOW_BUILD_FLOW_TREE_H_

#include "core_diagram.h"
#include "cpp_safe_ptr.h"
#include "flow_tree.h"

namespace esc::flow {
///
auto BuildFlowTree(const core::Diagram &diagram,
                   const cpp::SafeOwner &safe_owner) -> FlowTree;
///
auto CalculateNodeFlows(const FlowTree &flow_tree) -> NodeFlows;
}  // namespace esc::flow

#endif  // VH_FLOW_BUILD_FLOW_TREE_H_