#ifndef VH_PONC_FLOW_ALGORITHMS_H_
#define VH_PONC_FLOW_ALGORITHMS_H_

#include "core_diagram.h"
#include "cpp_callbacks.h"
#include "flow_node_flow.h"
#include "flow_tree.h"
#include "imgui_node_editor.h"

namespace vh::ponc::flow {
///
auto BuildFlowTree(const core::Diagram &diagram) -> FlowTree;
///
auto CalculateNodeFlows(
    const FlowTree &flow_tree,
    const cpp::Query<NodeFlow, ne::NodeId> &get_initial_node_flow) -> NodeFlows;
}  // namespace vh::ponc::flow

#endif  // VH_PONC_FLOW_ALGORITHMS_H_
