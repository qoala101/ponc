#ifndef VH_PONC_FLOW_ALGORITHMS_H_
#define VH_PONC_FLOW_ALGORITHMS_H_

#include <vector>

#include "core_diagram.h"
#include "cpp_callbacks.h"
#include "flow_node_flow.h"
#include "flow_tree_node.h"
#include "imgui_node_editor.h"

namespace vh::ponc::flow {
///
auto BuildFlowTrees(const core::Diagram &diagram) -> std::vector<TreeNode>;
///
auto CalculateNodeFlows(
    const std::vector<TreeNode> &flow_trees,
    const cpp::Query<NodeFlow, ne::NodeId> &get_initial_node_flow) -> NodeFlows;
}  // namespace vh::ponc::flow

#endif  // VH_PONC_FLOW_ALGORITHMS_H_
