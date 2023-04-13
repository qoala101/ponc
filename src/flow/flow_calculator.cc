#include "flow_calculator.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <ios>
#include <iostream>
#include <istream>
#include <iterator>
#include <limits>
#include <set>
#include <stack>
#include <unordered_set>
#include <utility>
#include <vector>

#include "core_i_family.h"
#include "core_id_generator.h"
#include "cpp_assert.h"
#include "cpp_scope.h"
#include "flow_tree.h"
#include "flow_tree_traversal.h"
#include "imgui_node_editor.h"

namespace vh::ponc::flow {
namespace {
void TraverseDepthFirstExConst(
    const TreeNodeEx &tree_node,
    const std::invocable<const TreeNodeEx &> auto &visitor_before_children,
    const std::invocable<const TreeNodeEx &> auto &visitor_after_children) {
  visitor_before_children(tree_node);

  for (const auto &child : tree_node.child_nodes) {
    TraverseDepthFirstExConst(child.second, visitor_before_children,
                              visitor_after_children);
  }

  visitor_after_children(tree_node);
}

void TraverseDepthFirstEx(
    TreeNodeEx &tree_node,
    const std::invocable<TreeNodeEx &> auto &visitor_before_children,
    const std::invocable<TreeNodeEx &> auto &visitor_after_children) {
  visitor_before_children(tree_node);

  for (auto &child : tree_node.child_nodes) {
    TraverseDepthFirstEx(child.second, visitor_before_children,
                         visitor_after_children);
  }

  visitor_after_children(tree_node);
}

auto InRange(int value, const Range<int> &range) {
  return (value >= range.min) && (value <= range.max);
}
}  // namespace

auto TreeNodeEx::FromFamily(const Family<int> &family_flow) -> TreeNodeEx {
  return {.family_id = family_flow.family_id,
          .outputs = family_flow.outputs,
          .cost = family_flow.cost};
}

auto TreeNodeEx::CalculateCost() const -> int {
  auto cost = 0;

  TraverseDepthFirstExConst(
      *this, [&cost](const auto &tree_node) { cost += tree_node.cost; },
      [](const auto &) {});

  return cost;
}

auto TreeNodeEx::EmplaceChild(int index, TreeNodeEx child) -> TreeNodeEx & {
  child.input = outputs[index];

  for (auto &output : child.outputs) {
    output += child.input;
  }

  return child_nodes[index] = std::move(child);
}

auto Calculator::Start(const CalculatorInput<int> &data)
    -> std::vector<TreeNodeEx> {
  auto root =
      TreeNodeEx{.family_id = data.input_family_id, .outputs = {data.input}};
  out_trees = std::vector<TreeNodeEx>{};
  min_output = data.output_range.min;
  client = TreeNodeEx{.family_id = data.output_family_id};
  RANGE = data.output_range;
  NUM_CLIENTS = data.num_outputs;

  RememberAlgStepSimple(data.family_flows, root, root);

  std::cout << "unique_outputs: ";
  for (const auto input : visited_outputs) {
    std::cout << input << ", ";
  }
  std::cout << "\n";

  Expects(!root.outputs.empty());
  auto &input_client_variants =
      output_tree_per_num_clients[root.outputs.front()];

  for (const auto &[i, tree] : input_client_variants) {
    auto &root_copy = out_trees.emplace_back(root);
    root_copy.child_nodes[0] = tree;
  }

  return out_trees;
}

void Calculator::MAKE_TEST_COMBINATION(
    const TreeNodeEx &node,
    std::map<int /*ouput_index*/, std::pair<int, TreeNodeEx>> &combination,
    int output_index, int clients_sum) {
  if (output_index < node.outputs.size()) {
    const auto output = node.outputs[output_index];
    const auto &output_combinations = output_tree_per_num_clients[output];

    for (auto output_combination_index =
             static_cast<int>(output_combinations.size());
         output_combination_index >= 0; --output_combination_index) {
      auto num_added_clients = 0;

      if (output_combination_index == 0) {
        combination.erase(output_index);
      } else {
        combination[output_index] = *std::next(output_combinations.begin(),
                                               output_combination_index - 1);
        num_added_clients = combination[output_index].first;
      }

      if ((clients_sum + num_added_clients) > NUM_CLIENTS) {
        continue;
      }

      clients_sum += num_added_clients;
      MAKE_TEST_COMBINATION(node, combination, output_index + 1, clients_sum);
      clients_sum -= num_added_clients;
    }

    return;
  }

  if (clients_sum <= 0) {
    return;
  }

  Expects(clients_sum <= NUM_CLIENTS);

  auto total_cost = node.cost;

  for (auto output_index = 0; output_index < node.outputs.size();
       ++output_index) {
    if (!combination.contains(output_index)) {
      continue;
    }

    const auto &[num_clients, tree] = combination.at(output_index);
    total_cost += tree.CalculateCost();
  }

  auto &input_client_variants = output_tree_per_num_clients[node.input];

  // need to add some choosing here when cost is equal. Atleast
  // PARAM_EQUAL_COST_CHOSE_MORE_AVERAGE
  if (input_client_variants.contains(clients_sum) &&
      input_client_variants.at(clients_sum).CalculateCost() <= total_cost) {
    return;
  }

  auto node_copy = node;

  for (auto output_index = 0; output_index < node.outputs.size();
       ++output_index) {
    if (!combination.contains(output_index)) {
      node_copy.child_nodes.erase(output_index);
      continue;
    }

    const auto &[num_clients, tree] = combination.at(output_index);
    node_copy.EmplaceChild(output_index, tree);
  }

  input_client_variants[clients_sum] = std::move(node_copy);
}

void Calculator::MAKE_TEST_COMBINATION_FOR_SAME_OUTPUTS(
    const TreeNodeEx &node,
    std::map<int /*ouput_index*/, std::pair<int, TreeNodeEx>> &combination,
    int start_with_combination, int output_index, int clients_sum) {
  if (output_index < node.outputs.size()) {
    const auto output = node.outputs[output_index];
    const auto &output_combinations = output_tree_per_num_clients[output];

    for (auto output_combination_index = start_with_combination;
         output_combination_index >= 0; --output_combination_index) {
      auto num_added_clients = 0;

      if (output_combination_index == 0) {
        combination.erase(output_index);
      } else {
        combination[output_index] = *std::next(output_combinations.begin(),
                                               output_combination_index - 1);
        num_added_clients = combination[output_index].first;
      }

      if ((clients_sum + num_added_clients) > NUM_CLIENTS) {
        continue;
      }

      clients_sum += num_added_clients;
      MAKE_TEST_COMBINATION_FOR_SAME_OUTPUTS(node, combination,
                                             output_combination_index,
                                             output_index + 1, clients_sum);
      clients_sum -= num_added_clients;
    }

    return;
  }

  if (clients_sum <= 0) {
    return;
  }

  Expects(clients_sum <= NUM_CLIENTS);

  auto total_cost = node.cost;

  for (auto output_index = 0; output_index < node.outputs.size();
       ++output_index) {
    if (!combination.contains(output_index)) {
      continue;
    }

    const auto &[num_clients, tree] = combination.at(output_index);
    total_cost += tree.CalculateCost();
  }

  auto &input_client_variants = output_tree_per_num_clients[node.input];

  if (input_client_variants.contains(clients_sum)) {
    const auto existing_variant_cost =
        input_client_variants.at(clients_sum).CalculateCost();

    if (existing_variant_cost < total_cost) {
      return;
    }

    if (existing_variant_cost == total_cost) {
      const auto PARAM_EQUAL_COST_CHOSE_MAX_CLIENTS_PER_OUTPUT = false;
      const auto PARAM_EQUAL_COST_CHOSE_MIN_CLIENTS_PER_OUTPUT =
          !PARAM_EQUAL_COST_CHOSE_MAX_CLIENTS_PER_OUTPUT;
      const auto PARAM_EQUAL_COST_CHOSE_MORE_AVERAGE = true;  // not implemented

      // here <= or < controls. <= means that first variant would be chosen,
      // where single output has max clients. < means that last one would be
      // chosen where clients are evenly split and also means that because we
      // traverse from 1x16 to 1x2, 1x2 would be chosen as it comes last

      if (PARAM_EQUAL_COST_CHOSE_MAX_CLIENTS_PER_OUTPUT) {
        return;
      }
    }
  }

  auto node_copy = node;

  for (auto output_index = 0; output_index < node.outputs.size();
       ++output_index) {
    if (!combination.contains(output_index)) {
      node_copy.child_nodes.erase(output_index);
      continue;
    }

    const auto &[num_clients, tree] = combination.at(output_index);
    node_copy.EmplaceChild(output_index, tree);
  }

  input_client_variants[clients_sum] = std::move(node_copy);
}

void Calculator::RememberAlgStepSimple(
    const std::vector<Family<int>> &family_flows, const TreeNodeEx &root,
    TreeNodeEx &node) {
  for (auto output_index = 0; output_index < node.outputs.size();
       ++output_index) {
    const auto output = node.outputs[output_index];

    if (output < min_output) {
      continue;
    }

    if (visited_outputs.contains(output)) {
      continue;
    }

    visited_outputs.emplace(output);

    if (InRange(output, RANGE)) {
      output_tree_per_num_clients[output].emplace(1, client);
    }

    // here we should chose how to traverse families.
    // we might sort them by the cost and then sort by other properties like
    // amount of pins

    for (auto family_i = 0; family_i < family_flows.size(); ++family_i) {
      auto &child = node.EmplaceChild(
          output_index, TreeNodeEx::FromFamily(family_flows[family_i]));

      ++DEPTH;
      RememberAlgStepSimple(family_flows, root, child);
      --DEPTH;
    }
  }

  const auto first_output = node.outputs.front();

  if (std::all_of(node.outputs.begin(), node.outputs.end(),
                  [first_output](const auto output) {
                    return output == first_output;
                  })) {
    const auto &output_combinations = output_tree_per_num_clients[first_output];

    auto combination = std::map<int, std::pair<int, TreeNodeEx>>{};
    MAKE_TEST_COMBINATION_FOR_SAME_OUTPUTS(node, combination,
                                           output_combinations.size(), 0, 0);
    return;
  }

  auto combination = std::map<int, std::pair<int, TreeNodeEx>>{};
  MAKE_TEST_COMBINATION(node, combination, 0, 0);
}
}  // namespace vh::ponc::flow