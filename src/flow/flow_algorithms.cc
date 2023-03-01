#include "flow_algorithms.h"

#include <unordered_map>
#include <unordered_set>

#include "cpp_assert.h"

namespace esc::flow {
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

    if (pin_flows.input_pin_flow.has_value() &&
        HasLinkFromParent(pin_flows, links)) {
      continue;
    }

    auto &tree_node = root_nodes.emplace_back();
    tree_node.node = node->CreateSafePointer();
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
auto FindLinkFromParentToChild(
    const std::unordered_map<uintptr_t, float> &parent_output_pins,
    ne::PinId child_input_pin, const std::vector<core::Link> &links)
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
void CalculateNodeFlow(std::unordered_map<uintptr_t, NodeFlow> &node_flows_,
                       const TreeNode &node, float input_from_parent = 0.F) {
  const auto node_id = node.node->GetId();

  auto calculated_flow = node_flows_.find(node_id.Get());
  const auto initial_flow = node.node->GetInitialFlow();

  if (calculated_flow == node_flows_.end()) {
    calculated_flow = node_flows_.emplace(node_id, initial_flow).first;
  } else {
    calculated_flow->second += initial_flow;
  }

  calculated_flow->second += input_from_parent;

  for (const auto &[child_pin, child_node] : node.child_nodes) {
    Expects(calculated_flow->second.output_pin_flows.contains(child_pin));
    const auto added_flow =
        calculated_flow->second.output_pin_flows.at(child_pin);

    CalculateNodeFlow(node_flows_, child_node, added_flow);
  }
}
}  // namespace

///
auto BuildFlowTree(const core::Diagram &diagram) -> FlowTree {
  const auto &links = diagram.GetLinks();
  const auto &nodes = diagram.GetNodes();

  auto flow_tree = FlowTree{FindRootNodes(nodes, links)};
  auto visited_nodes = std::unordered_set<const core::INode *>{};
  auto current_level_tree_nodes = std::vector<TreeNode *>{};

  for (auto &root_node : flow_tree.root_nodes) {
    visited_nodes.emplace(&*root_node.node);
    current_level_tree_nodes.emplace_back(&root_node);
  }

  while (!current_level_tree_nodes.empty()) {
    const auto previous_level_tree_nodes = std::move(current_level_tree_nodes);

    for (const auto &node : nodes) {
      if (visited_nodes.contains(node.get())) {
        continue;
      }

      const auto &node_input_pin = node->GetInitialFlow().input_pin_flow;
      Expects(node_input_pin.has_value());

      for (const auto &possible_parent : previous_level_tree_nodes) {
        const auto parent_output_pins =
            possible_parent->node->GetInitialFlow().output_pin_flows;

        const auto link_to_parent = FindLinkFromParentToChild(
            parent_output_pins, node_input_pin->first, links);

        if (!link_to_parent.has_value()) {
          continue;
        }

        auto &[pin_id, tree_node] =
            *possible_parent->child_nodes
                 .emplace((*link_to_parent)->start_pin_id,
                          TreeNode{node->CreateSafePointer()})
                 .first;

        current_level_tree_nodes.emplace_back(&tree_node);
        visited_nodes.emplace(node.get());
        break;
      }
    }
  }

  return flow_tree;
}

///
auto CalculateNodeFlows(const FlowTree &flow_tree)
    -> std::unordered_map<uintptr_t, NodeFlow> {
  auto node_flows = std::unordered_map<uintptr_t, NodeFlow>{};

  for (const auto &node : flow_tree.root_nodes) {
    CalculateNodeFlow(node_flows, node);
  }

  return node_flows;
}
}  // namespace esc::flow