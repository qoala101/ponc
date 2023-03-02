#ifndef VH_FLOW_BUILD_FLOW_TREE_H_
#define VH_FLOW_BUILD_FLOW_TREE_H_

#include "core_diagram.h"
#include "cpp_safe_pointer.h"
#include "flow_tree.h"

namespace esc::flow {
///
auto BuildFlowTree(const core::Diagram &diagram,
                   const cpp::SafePointerFactory &safe_pointer_factory)
    -> FlowTree;
///
auto CalculateNodeFlows(const FlowTree &flow_tree) -> NodeFlows;
}  // namespace esc::flow

#endif  // VH_FLOW_BUILD_FLOW_TREE_H_
