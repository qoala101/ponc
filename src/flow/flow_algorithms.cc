/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "flow_algorithms.h"

#include <imgui_node_editor.h>

#include <algorithm>
#include <iterator>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <unordered_set>
#include <utility>

#include "core_diagram.h"
#include "core_i_node.h"
#include "core_id_value.h"
#include "core_link.h"
#include "cpp_assert.h"
#include "flow_node_flow.h"
#include "flow_tree_node.h"
#include "flow_tree_traversal.h"

namespace vh::ponc::flow {
namespace {
///
auto HasLinkFromParent(const NodeFlow &node_flow,
                       const std::vector<core::Link> &links) {
  Expects(node_flow.input_pin_flow.has_value());

  return std::any_of(
      links.cbegin(), links.cend(), [&node_flow](const auto &link) {
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
      root_nodes.emplace_back(TreeNode{node->GetId()});
    }
  }

  return root_nodes;
}

///
auto FindLinkConnectingPins(ne::PinId start_pin, ne::PinId end_pin,
                            const std::vector<core::Link> &links)
    -> std::optional<const core::Link *> {
  const auto link = std::find_if(
      links.cbegin(), links.cend(), [start_pin, end_pin](const auto &link) {
        return (link.start_pin_id == start_pin) && (link.end_pin_id == end_pin);
      });

  if (link == links.cend()) {
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

  if (calculated_flow == node_flows_.cend()) {
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

///
auto FindNodeIfExists(const core::Diagram &diagram, ne::NodeId node_id)
    -> std::optional<const core::INode *> {
  const auto &nodes = diagram.GetNodes();
  const auto node = std::find_if(
      nodes.cbegin(), nodes.cend(),
      [node_id](const auto &node) { return node->GetId() == node_id; });

  if (node == nodes.cend()) {
    return std::nullopt;
  }

  return &**node;
}

///
auto DoFlowTreesMatchDiagram(const std::vector<TreeNode> &flow_trees,
                             const core::Diagram &diagram) {
  auto child_is_missing = false;
  auto flow_tree_node_ids = std::set<core::IdValue<ne::NodeId>>{};
  auto flow_tree_link_pins =
      std::set<std::pair<core::IdValue<ne::PinId>, core::IdValue<ne::PinId>>>{};

  for (const auto &flow_tree : flow_trees) {
    TraverseDepthFirst(
        flow_tree,
        [&diagram, &child_is_missing, &flow_tree_node_ids,
         &flow_tree_link_pins](const auto &tree_node) {
          if (child_is_missing) {
            return;
          }

          flow_tree_node_ids.emplace(tree_node.node_id);

          for (const auto &[pin_to_child, child_node] : tree_node.child_nodes) {
            const auto child = FindNodeIfExists(diagram, child_node.node_id);

            if (!child.has_value()) {
              child_is_missing = true;
              break;
            }

            const auto &child_pin =
                core::INode::GetFirstPinOfKind(**child, ne::PinKind::Input);

            flow_tree_link_pins.emplace(pin_to_child, child_pin);
          }
        },
        [](const auto &) {});
  }

  if (child_is_missing) {
    return false;
  }

  const auto &nodes = diagram.GetNodes();

  auto diagram_node_ids = std::set<core::IdValue<ne::NodeId>>{};
  std::transform(nodes.cbegin(), nodes.cend(),
                 std::inserter(diagram_node_ids, diagram_node_ids.cbegin()),
                 [](const auto &node) { return node->GetId().Get(); });

  if (diagram_node_ids != flow_tree_node_ids) {
    return false;
  }

  const auto &links = diagram.GetLinks();

  auto diagram_link_pins =
      std::set<std::pair<core::IdValue<ne::PinId>, core::IdValue<ne::PinId>>>{};
  std::transform(
      links.cbegin(), links.cend(),
      std::inserter(diagram_link_pins, diagram_link_pins.cbegin()),
      [](const auto &link) {
        return std::pair{link.start_pin_id.Get(), link.end_pin_id.Get()};
      });

  return diagram_link_pins == flow_tree_link_pins;
}
}  // namespace

///
auto BuildFlowTrees(const core::Diagram &diagram) -> std::vector<TreeNode> {
  const auto &links = diagram.GetLinks();
  const auto &nodes = diagram.GetNodes();

  auto root_nodes = FindRootNodes(nodes, links);
  auto visited_nodes = std::unordered_set<core::IdValue<ne::NodeId>>{};

  auto current_level_tree_nodes = std::vector<TreeNode *>{};
  current_level_tree_nodes.reserve(root_nodes.size());

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
                 .emplace((*link_to_parent)->start_pin_id, TreeNode{node_id})
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
void RebuildFlowTrees(const core::Diagram &diagram,
                      std::vector<TreeNode> &flow_trees) {
  if (!DoFlowTreesMatchDiagram(flow_trees, diagram)) {
    flow_trees = BuildFlowTrees(diagram);
  }
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