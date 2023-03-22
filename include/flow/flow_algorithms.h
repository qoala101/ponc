#ifndef VH_PONC_FLOW_ALGORITHMS_H_
#define VH_PONC_FLOW_ALGORITHMS_H_

#include "core_diagram.h"
#include "cpp_safe_ptr.h"
#include "flow_tree.h"

namespace vh::ponc::flow {
///
auto BuildFlowTree(const core::Diagram &diagram,
                   const cpp::SafeOwner &safe_owner) -> FlowTree;
///
auto CalculateNodeFlows(const FlowTree &flow_tree) -> NodeFlows;
}  // namespace vh::ponc::flow

#endif  // VH_PONC_FLOW_ALGORITHMS_H_
