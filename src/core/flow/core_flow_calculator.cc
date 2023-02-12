#include "core_flow_calculator.h"

#include <bits/ranges_algo.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <unordered_set>
#include <vector>

#include "core_flow.h"
#include "core_i_node.h"
#include "esc_state.h"

namespace esc::core {
namespace {
auto HasParentPin(const NodeFlow &node_flow) {
  return node_flow.parent_flow.has_value();
}

auto HasLinkFromParent(const NodeFlow &node_flow,
                       const std::vector<Link> &links) {
  return std::ranges::any_of(links, [&node_flow](const auto &link) {
    return link.end_pin_id == node_flow.parent_flow->id;
  });
}

auto IsParent(const NodeFlow &parent, const NodeFlow &child,
              const std::vector<Link> &links) {
  return std::ranges::any_of(links, [&parent, &child](const auto &link) {
    if (link.end_pin_id != child.parent_flow->id) {
      return false;
    }

    return std::ranges::any_of(parent.child_flows,
                               [&link](const auto &child_flow) {
                                 return child_flow.id == link.start_pin_id;
                               });
  });
}
}  // namespace

void FlowCalculator::OnFrame(const State &state) {
  // auto node_flows =
  //     std::unordered_map<const std::shared_ptr<INode> *, NodeFlow>{};

  // for (auto &family : state.app_.GetDiagram().GetFamilies()) {
  //   for (auto &node : family->GetNodes()) {
  //     const auto &initial_node_flow =
  //         node_flows.emplace(&node, node->GetInitialFlow()).first->second;

  //     if (initial_node_flow.parent_flow.has_value()) {
  //       pin_values_.emplace(initial_node_flow.parent_flow->id.Get(),
  //                           initial_node_flow.parent_flow->value);
  //     }

  //     for (const auto &child_value : initial_node_flow.child_flows) {
  //       pin_values_.emplace(child_value.id.Get(), child_value.value);
  //     }
  //   }
  // }
  pin_values_.clear();
  RebuildFlowTree(state);
  CalculateFlowValues(state);
}

auto FlowCalculator::GetFlowTree() const -> const Tree & { return flow_tree_; }

auto FlowCalculator::GetCalculatedFlow(const INode &node) const -> NodeFlow {
  auto flow_values = node.GetInitialFlow();

  if (flow_values.parent_flow.has_value()) {
    flow_values.parent_flow->value =
        pin_values_.at(flow_values.parent_flow->id.Get());
  }

  for (auto &child_pin_value : flow_values.child_flows) {
    child_pin_value.value = pin_values_.at(child_pin_value.id.Get());
  }

  return flow_values;
}

void FlowCalculator::RebuildFlowTree(const State &state) {
  flow_tree_.root_nodes.clear();

  auto node_flows =
      std::unordered_map<const std::shared_ptr<INode> *, NodeFlow>{};

  for (const auto &family : state.app_.GetDiagram().GetFamilies()) {
    for (const auto &node : family->GetNodes()) {
      // node_flows.emplace(&node, node->GetInitialFlow());

      const auto &initial_node_flow =
          node_flows.emplace(&node, node->GetInitialFlow()).first->second;

      if (initial_node_flow.parent_flow.has_value()) {
        pin_values_.emplace(initial_node_flow.parent_flow->id.Get(),
                            initial_node_flow.parent_flow->value);
      }

      for (const auto &child_value : initial_node_flow.child_flows) {
        pin_values_.emplace(child_value.id.Get(), child_value.value);
      }
    }
  }

  auto visited_nodes = std::unordered_set<const std::shared_ptr<INode> *>{};
  auto current_level_tree_nodes =
      std::unordered_map<const std::shared_ptr<INode> *,
                         std::vector<TreeNode> *>{};

  const auto &links = state.app_.GetDiagram().GetLinks();

  for (const auto &flow : node_flows) {
    const auto &[node, node_flow] = flow;

    if (HasParentPin(node_flow) && HasLinkFromParent(node_flow, links)) {
      continue;
    }

    auto &tree_node = flow_tree_.root_nodes.emplace_back(TreeNode{*node});
    current_level_tree_nodes.emplace(node, &tree_node.child_nodes);
    visited_nodes.emplace(node);
  }

  auto visited_node_ids_size = 0;

  while (const auto more_nodes_visited =
             visited_node_ids_size < static_cast<int>(visited_nodes.size())) {
    visited_node_ids_size = static_cast<int>(visited_nodes.size());

    auto previous_level_tree_nodes =
        std::unordered_map<const std::shared_ptr<INode> *,
                           std::vector<TreeNode> *>{};

    swap(current_level_tree_nodes, previous_level_tree_nodes);

    for (const auto &flow : node_flows) {
      const auto &[node, node_flow] = flow;

      if (visited_nodes.contains(node)) {
        continue;
      }

      auto previous_level_tree_node = std::ranges::find_if(
          previous_level_tree_nodes,
          [&node_flows, &flow, &links](const auto &previous_level_tree_node) {
            return IsParent(node_flows.at(previous_level_tree_node.first),
                            flow.second, links);
          });

      if (previous_level_tree_node == previous_level_tree_nodes.end()) {
        continue;
      }

      auto &tree_node =
          previous_level_tree_node->second->emplace_back(TreeNode{*node});
      current_level_tree_nodes.emplace(node, &tree_node.child_nodes);

      visited_nodes.emplace(node);
    }
  }
}

void FlowCalculator::CalculateFlowValues(const State &state) {}
}  // namespace esc::core