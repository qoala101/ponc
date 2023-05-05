#include "flow_algorithms.h"

#include <cstdint>
#include <unordered_map>
#include <unordered_set>

#include "core_diagram.h"
#include "cpp_assert.h"
#include "flow_node_flow.h"
#include "flow_tree_node.h"
#include "imgui_node_editor.h"

namespace vh::ponc::flow {
namespace {
///
auto HasLinkFromParent(const NodeFlow &node_flow,
                       const std::vector<core::Link> &links) {
  Expects(node_flow.input_pin_flow.has_value());

  return std::any_of(
      links.begin(), links.end(), [&node_flow](const auto &link) {
        return link.end_pin_id.Get() == node_flow.input_pin_flow->first;
      });
}

///
auto FindRootNodes(const std::vector<std::unique_ptr<core::INode>> &nodes,
                   const std::vector<core::Link> &links) {
  auto root_nodes = std::vector<TreeNode>{};

  for (const auto &node : nodes) {
    const auto pin_flows = node->GetInitialFlow();

    if (!pin_flows.input_pin_flow.has_value() ||
        !HasLinkFromParent(pin_flows, links)) {
      root_nodes.emplace_back(TreeNode{.node_id = node->GetId()});
    }
  }

  return root_nodes;
}

///
auto FindLinkConnectingPins(ne::PinId start_pin, ne::PinId end_pin,
                            const std::vector<core::Link> &links)
    -> std::optional<const core::Link *> {
  const auto link = std::find_if(
      links.begin(), links.end(), [start_pin, end_pin](const auto &link) {
        return (link.start_pin_id == start_pin) && (link.end_pin_id == end_pin);
      });

  if (link == links.end()) {
    return std::nullopt;
  }

  return &*link;
}

///
auto FindLinkFromParentToChild(const PinFlows &parent_output_pins,
                               ne::PinId child_input_pin,
                               const std::vector<core::Link> &links)
    -> std::optional<const core::Link *> {
  for (const auto &[parent_output_pin, value] : parent_output_pins) {
    const auto link =
        FindLinkConnectingPins(parent_output_pin, child_input_pin, links);

    if (link.has_value()) {
      return link;
    }
  }

  return std::nullopt;
}

///
// NOLINTNEXTLINE(*-no-recursion)
void CalculateNodeFlow(
    flow::NodeFlows &node_flows_, const TreeNode &node,
    const cpp::Query<NodeFlow, ne::NodeId> &get_initial_node_flow,
    float input_from_parent = 0) {
  auto calculated_flow = node_flows_.find(node.node_id.Get());
  const auto initial_flow = get_initial_node_flow(node.node_id);

  if (calculated_flow == node_flows_.end()) {
    calculated_flow = node_flows_.emplace(node.node_id, initial_flow).first;
  } else {
    calculated_flow->second += initial_flow;
  }

  calculated_flow->second += input_from_parent;

  for (const auto &[child_pin, child_node] : node.child_nodes) {
    Expects(calculated_flow->second.output_pin_flows.contains(child_pin));
    const auto added_flow =
        calculated_flow->second.output_pin_flows.at(child_pin);

    CalculateNodeFlow(node_flows_, child_node, get_initial_node_flow,
                      added_flow);
  }
}
}  // namespace

///
auto BuildFlowTrees(const core::Diagram &diagram) -> std::vector<TreeNode> {
  const auto &links = diagram.GetLinks();
  const auto &nodes = diagram.GetNodes();

  auto root_nodes = FindRootNodes(nodes, links);
  auto visited_nodes = std::unordered_set<core::IdValue<ne::NodeId>>{};
  auto current_level_tree_nodes = std::vector<TreeNode *>{};

  for (auto &root_node : root_nodes) {
    visited_nodes.emplace(root_node.node_id);
    current_level_tree_nodes.emplace_back(&root_node);
  }

  while (!current_level_tree_nodes.empty()) {
    const auto previous_level_tree_nodes = std::move(current_level_tree_nodes);

    for (const auto &node : nodes) {
      const auto node_id = node->GetId();

      if (visited_nodes.contains(node_id.Get())) {
        continue;
      }

      const auto &node_input_pin = node->GetInitialFlow().input_pin_flow;
      Expects(node_input_pin.has_value());

      for (const auto &possible_parent : previous_level_tree_nodes) {
        const auto &parent_node =
            core::Diagram::FindNode(diagram, possible_parent->node_id);
        const auto parent_output_pins =
            parent_node.GetInitialFlow().output_pin_flows;
        const auto link_to_parent = FindLinkFromParentToChild(
            parent_output_pins, node_input_pin->first, links);

        if (!link_to_parent.has_value()) {
          continue;
        }

        auto &[pin_id, tree_node] =
            *possible_parent->child_nodes
                 .emplace((*link_to_parent)->start_pin_id,
                          TreeNode{.node_id = node_id})
                 .first;

        current_level_tree_nodes.emplace_back(&tree_node);
        visited_nodes.emplace(node_id);
        break;
      }
    }
  }

  return root_nodes;
}

///
auto CalculateNodeFlows(
    const std::vector<TreeNode> &flow_trees,
    const cpp::Query<NodeFlow, ne::NodeId> &get_initial_node_flow)
    -> flow::NodeFlows {
  auto node_flows = flow::NodeFlows{};

  for (const auto &flow_tree : flow_trees) {
    CalculateNodeFlow(node_flows, flow_tree, get_initial_node_flow);
  }

  return node_flows;
}
}  // namespace vh::ponc::flow