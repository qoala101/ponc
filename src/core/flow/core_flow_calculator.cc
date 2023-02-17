#include "core_flow_calculator.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <ranges>
#include <unordered_set>
#include <vector>

#include "app_state.h"
#include "core_diagram.h"
#include "core_flow.h"
#include "core_i_node.h"
#include "core_link.h"
#include "cpp_assert.h"
#include "imgui_node_editor.h"

namespace esc::core {
namespace {
// ---
auto HasInputPin [[nodiscard]] (const Flow &node_flow) {
  return node_flow.input_pin_flow.has_value();
}

// ---
auto HasLinkFromParent
    [[nodiscard]] (const Flow &node_flow, const std::vector<Link> &links) {
  Expects(node_flow.input_pin_flow.has_value());

  return std::ranges::any_of(links, [&node_flow](const auto &link) {
    return link.end_pin_id.Get() == node_flow.input_pin_flow->first;
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
    [[nodiscard]] (const std::map<uintptr_t, float> &parent_output_pins,
                   ne::PinId child_input_pin, const std::vector<Link> &links)
    -> const Link * {
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

// ---
void FlowCalculator::OnFrame(const State &state) {
  const auto &diagram = state.diagram_;
  const auto &families = diagram.GetFamilies();
  const auto &links = diagram.GetLinks();
  RebuildFlowTree(families, links);
  RecalculateFlowValues();
}

// ---
auto FlowCalculator::GetFlowTree() const -> const Tree & { return flow_tree_; }

// ---
auto FlowCalculator::GetCalculatedFlow(const INode &node) const
    -> const Flow & {
  const auto node_id = node.GetId().Get();
  Expects(node_flows_.contains(node_id));
  return node_flows_.at(node_id);
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
  }
}

// ---
void FlowCalculator::RecalculateFlowValues() {
  node_flows_.clear();

  for (const auto &node : flow_tree_.root_nodes) {
    CalculateFlowValuesForNode(node);
  }
}

// ---
// NOLINTNEXTLINE(*-no-recursion)
void FlowCalculator::CalculateFlowValuesForNode(const TreeNode &node,
                                                float input_from_parent) {
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
    const auto added_flow =
        calculated_flow->second.output_pin_flows.at(child_pin);

    CalculateFlowValuesForNode(child_node, added_flow);
  }
}
}  // namespace esc::core