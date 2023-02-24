#include "flow_tree.h"

#include <bits/ranges_algo.h>

#include <iostream>
#include <unordered_set>

#include "cpp_assert.h"

namespace esc::flow {
namespace {
auto HasInputPin(const NodeFlow &node_flow) {
  return node_flow.input_pin_flow.has_value();
}

auto HasLinkFromParent(const NodeFlow &node_flow,
                       const std::vector<core::Link> &links) {
  Expects(node_flow.input_pin_flow.has_value());

  return std::ranges::any_of(links, [&node_flow](const auto &link) {
    return link.end_pin_id.Get() == node_flow.input_pin_flow->first;
  });
}

auto FindRootNodes(const std::vector<std::shared_ptr<core::INode>> &nodes,
                   const std::vector<core::Link> &links) {
  auto root_nodes = std::vector<TreeNode>{};

  for (const auto &node : nodes) {
    const auto pin_flows = node->GetInitialFlow();

    if (HasInputPin(pin_flows) && HasLinkFromParent(pin_flows, links)) {
      continue;
    }

    root_nodes.emplace_back(TreeNode{node});
  }

  return root_nodes;
}

auto FindLinkConnectingPins(ne::PinId start_pin, ne::PinId end_pin,
                            const std::vector<core::Link> &links)
    -> const core::Link * {
  const auto link =
      std::ranges::find_if(links, [start_pin, end_pin](const auto &link) {
        return (link.start_pin_id == start_pin) && (link.end_pin_id == end_pin);
      });

  if (link == links.end()) {
    return nullptr;
  }

  return &*link;
}

auto FindLinkFromParentToChild(
    const std::map<uintptr_t, float> &parent_output_pins,
    ne::PinId child_input_pin, const std::vector<core::Link> &links)
    -> const core::Link * {
  for (const auto &[parent_output_pin, value] : parent_output_pins) {
    const auto *const link =
        FindLinkConnectingPins(parent_output_pin, child_input_pin, links);

    if (link != nullptr) {
      return link;
    }
  }

  return nullptr;
}
}  // namespace

auto FindById::operator()(const TreeNode &tree_node) const -> bool {
  return tree_node.node->GetId() == node_id;
}

auto BuildFlowTree(const core::Diagram &diagram) -> Tree {
  const auto &links = diagram.GetLinks();
  const auto &nodes = diagram.GetNodes();

  auto flow_tree = Tree{FindRootNodes(nodes, links)};
  auto visited_nodes = std::unordered_set<const core::INode *>{};
  auto current_level_tree_nodes = std::vector<TreeNode *>{};

  for (auto &root_node : flow_tree.root_nodes) {
    visited_nodes.emplace(root_node.node.get());
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

        const auto *link_to_parent = FindLinkFromParentToChild(
            parent_output_pins, node_input_pin->first, links);

        if (link_to_parent == nullptr) {
          continue;
        }

        auto &[pin_id, tree_node] =
            *possible_parent->child_nodes
                 .emplace(link_to_parent->start_pin_id, TreeNode{node})
                 .first;

        current_level_tree_nodes.emplace_back(&tree_node);
        visited_nodes.emplace(node.get());
        break;
      }
    }
  }

  return flow_tree;
}
}  // namespace esc::flow