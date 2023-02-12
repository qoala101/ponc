#include "core_flow_calculator.h"

#include <bits/ranges_algo.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <unordered_set>
#include <vector>

#include "core_diagram.h"
#include "core_flow.h"
#include "core_i_node.h"
#include "core_link.h"
#include "cpp_assert.h"
#include "esc_state.h"
#include "imgui_node_editor.h"

namespace esc::core {
namespace {
// ---
auto HasInputPin [[nodiscard]] (const NodePinFlows &node_flow) {
  return node_flow.input_pin_flow.has_value();
}

// ---
auto HasLinkFromParent [[nodiscard]] (const NodePinFlows &node_flow,
                                      const std::vector<Link> &links) {
  return std::ranges::any_of(links, [&node_flow](const auto &link) {
    return link.end_pin_id == node_flow.input_pin_flow->id;
  });
}

// ---
auto FindRootNodes
    [[nodiscard]] (const std::vector<std::shared_ptr<IFamily>> &families,
                   const std::vector<Link> &links) {
  auto root_nodes = std::vector<TreeNode>{};

  for (const auto &family : families) {
    for (const auto &node : family->GetNodes()) {
      const auto pin_flows = node->GetInitialFlow();

      if (HasInputPin(pin_flows) && HasLinkFromParent(pin_flows, links)) {
        continue;
      }

      root_nodes.emplace_back(TreeNode{node});
    }
  }

  return root_nodes;
}

// ---
auto FindLinkConnectingPins
    [[nodiscard]] (ne::PinId start_pin, ne::PinId end_pin,
                   const std::vector<Link> &links) -> const Link * {
  const auto link =
      std::ranges::find_if(links, [start_pin, end_pin](const auto &link) {
        return (link.start_pin_id == start_pin) && (link.end_pin_id == end_pin);
      });

  if (link == links.end()) {
    return nullptr;
  }

  return &*link;
}

// ---
auto FindLinkFromParentToChild
    [[nodiscard]] (const std::vector<PinFlow> &parent_output_pins,
                   ne::PinId child_input_pin, const std::vector<Link> &links)
    -> const Link * {
  for (const auto &parent_output_pin : parent_output_pins) {
    const auto *const link =
        FindLinkConnectingPins(parent_output_pin.id, child_input_pin, links);

    if (link != nullptr) {
      return link;
    }
  }

  return nullptr;
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
  // pin_values_.clear();
  const auto &diagram = state.app_.GetDiagram();
  const auto &families = diagram.GetFamilies();
  const auto &links = diagram.GetLinks();
  RebuildFlowTree(families, links);
  CalculateFlowValues(state);
}

auto FlowCalculator::GetFlowTree() const -> const Tree & { return flow_tree_; }

auto FlowCalculator::GetCalculatedFlow(const INode &node) const
    -> NodePinFlows {
  auto flow_values = node.GetInitialFlow();

  if (flow_values.input_pin_flow.has_value()) {
    flow_values.input_pin_flow->value =
        pin_values_.at(flow_values.input_pin_flow->id.Get());
  }

  for (auto &child_pin_value : flow_values.output_pin_flows) {
    child_pin_value.value = pin_values_.at(child_pin_value.id.Get());
  }

  return flow_values;
}

// ---
void FlowCalculator::RebuildFlowTree(
    const std::vector<std::shared_ptr<IFamily>> &families,
    const std::vector<Link> &links) {
  flow_tree_.root_nodes = FindRootNodes(families, links);

  auto visited_nodes = std::unordered_set<const INode *>{};
  auto current_level_tree_nodes = std::vector<TreeNode *>{};

  for (auto &root_node : flow_tree_.root_nodes) {
    visited_nodes.emplace(root_node.node.get());
    current_level_tree_nodes.emplace_back(&root_node);
  }

  while (!current_level_tree_nodes.empty()) {
    const auto previous_level_tree_nodes = std::move(current_level_tree_nodes);

    for (const auto &family : families) {
      for (const auto &node : family->GetNodes()) {
        if (visited_nodes.contains(node.get())) {
          continue;
        }

        const auto &node_input_pin = node->GetInitialFlow().input_pin_flow;
        cpp::Expects(node_input_pin.has_value());

        for (const auto &possible_parent : previous_level_tree_nodes) {
          const auto parent_output_pins =
              possible_parent->node->GetInitialFlow().output_pin_flows;

          const auto *link_to_parent = FindLinkFromParentToChild(
              parent_output_pins, node_input_pin->id, links);

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
  }
}

void FlowCalculator::VisitNode(const TreeNode &node, const State &state) {
  // const auto initial_node_flow = node.node->GetInitialFlow();

  // if (initial_node_flow.parent_flow.has_value()) {
  //   pin_values_[initial_node_flow.parent_flow->id.Get()] +=
  //       initial_node_flow.parent_flow->value;
  // }

  // const auto &links = state.app_.GetDiagram().GetLinks();

  // for (const auto &child_value : initial_node_flow.child_flows) {
  //   pin_values_[child_value.id.Get()] += child_value.value;

  //   for (const auto &child_node : node.child_nodes) {
  //     const auto child_pin_id =
  //         child_node.node->GetInitialFlow().parent_flow->id;

  //     if (HasLink(child_value.id, child_pin_id, links)) {
  //       pin_values_[child_pin_id.Get()] += child_value.value;
  //     }
  //   }
  // }

  // for (const auto &child_node : node.child_nodes) {
  //   VisitNode(child_node, state);
  // }
}

void FlowCalculator::CalculateFlowValues(const State &state) {
  // pin_values_.clear();
  // node_flows_.clear();

  // for (const auto &node : flow_tree_.root_nodes) {
  //   VisitNode(node, state);
  // }
}
}  // namespace esc::core