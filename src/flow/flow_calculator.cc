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
auto InRange(int value, const Range<int> &range) {
  return (value >= range.min) && (value <= range.max);
}
}  // namespace

auto TreeNodeEx::FromFamily(const Family<int> &family_flow) -> TreeNodeEx {
  auto a = TreeNodeEx{};

  a.family_id = family_flow.family_id;
  a.outputs = family_flow.outputs;
  a.node_cost_ = family_flow.cost;

  return a;
}

auto TreeNodeEx::GetNodeCost() const -> int { return node_cost_; }

auto TreeNodeEx::GetTreeCost() const -> int { return tree_cost_; }

void TreeNodeEx::SetTreeCost(int cost) { tree_cost_ = cost; }

auto TreeNodeEx::GetChildren() const -> const std::map<int, TreeNodeEx> & {
  return child_nodes;
}

auto TreeNodeEx::EmplaceChild(int index, TreeNodeEx child) -> TreeNodeEx & {
  EraseChild(index);

  child.input = outputs[index];

  for (auto &output : child.outputs) {
    output += child.input;
  }

  return child_nodes[index] = std::move(child);
}

void TreeNodeEx::EraseChild(int index) {
  if (!child_nodes.contains(index)) {
    return;
  }

  child_nodes.erase(index);
}

auto Calculator::Start(const CalculatorInput<int> &data)
    -> std::vector<TreeNodeEx> {
  data_ = data;

  auto root = TreeNodeEx{};
  root.family_id = data.input_family_id;
  root.outputs = {data.input};

  out_trees = std::vector<TreeNodeEx>{};

  client_node_ = TreeNodeEx{};
  client_node_.family_id = data.output_family_id;

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
    root_copy.EmplaceChild(0, tree);
  }

  return out_trees;
}

void Calculator::MakeCombination(
    const TreeNodeEx &node,
    std::map<int /*ouput_index*/, std::pair<int /*num_clients*/, TreeNodeEx>>
        &combination,
    int output_index, int start_with_combination, int clients_sum,
    int clients_cost, bool same_outputs) {
  auto &input_client_variants = output_tree_per_num_clients[node.input];

  if (output_index < static_cast<int>(node.outputs.size())) {
    const auto output = node.outputs[output_index];
    const auto &output_combinations = output_tree_per_num_clients[output];

    for (auto output_combination_index =
             same_outputs ? start_with_combination
                          : static_cast<int>(output_combinations.size());
         output_combination_index >= 0; --output_combination_index) {
      if (combination.contains(output_index)) {
        clients_sum -= combination.at(output_index).first;
        clients_cost -= combination.at(output_index).second.GetTreeCost();
      }

      if (output_combination_index == 0) {
        combination.erase(output_index);
      } else {
        combination[output_index] = *std::next(output_combinations.begin(),
                                               output_combination_index - 1);
        clients_sum += combination[output_index].first;
        clients_cost += combination[output_index].second.GetTreeCost();
      }

      if (clients_sum > data_.num_outputs) {
        continue;
      }

      if (input_client_variants.contains(clients_sum) &&
          (clients_cost >
           input_client_variants.at(clients_sum).GetTreeCost())) {
        continue;
      }

      MakeCombination(node, combination, output_index + 1,
                      output_combination_index, clients_sum, clients_cost,
                      same_outputs);
    }

    return;
  }

  if (clients_sum <= 0) {
    return;
  }

  auto total_cost = node.GetNodeCost() + clients_cost;

  if (same_outputs) {
    if (input_client_variants.contains(clients_sum)) {
      const auto existing_variant_cost =
          input_client_variants.at(clients_sum).GetTreeCost();

      if (existing_variant_cost < total_cost) {
        return;
      }

      if (existing_variant_cost == total_cost) {
        const auto PARAM_EQUAL_COST_CHOSE_MAX_CLIENTS_PER_OUTPUT = false;
        const auto PARAM_EQUAL_COST_CHOSE_MIN_CLIENTS_PER_OUTPUT =
            !PARAM_EQUAL_COST_CHOSE_MAX_CLIENTS_PER_OUTPUT;
        const auto PARAM_EQUAL_COST_CHOSE_MORE_AVERAGE =
            false;  // not implemented
        const auto PARAM_EQUAL_COST_CHOSE_LESS_FREE_OUTPUTS =
            false;  // not implemented

        // here <= or < controls. <= means that first variant would be chosen,
        // where single output has max clients. < means that last one would be
        // chosen where clients are evenly split and also means that because we
        // traverse from 1x16 to 1x2, 1x2 would be chosen as it comes last

        // VH: something makes sense only for same_outputs?

        if (PARAM_EQUAL_COST_CHOSE_MAX_CLIENTS_PER_OUTPUT) {
          return;
        }
      }
    }
  } else {
    // need to add some choosing here when cost is equal. Atleast
    // PARAM_EQUAL_COST_CHOSE_MORE_AVERAGE
    if (input_client_variants.contains(clients_sum) &&
        input_client_variants.at(clients_sum).GetTreeCost() <= total_cost) {
      return;
    }
  }

  auto node_copy = node;

  for (auto output_index = 0; output_index < node.outputs.size();
       ++output_index) {
    if (!combination.contains(output_index)) {
      node_copy.EraseChild(output_index);
      continue;
    }

    const auto &[num_clients, tree] = combination.at(output_index);
    node_copy.EmplaceChild(output_index, tree);
  }

  node_copy.SetTreeCost(total_cost);
  input_client_variants[clients_sum] = std::move(node_copy);
}

void Calculator::RememberAlgStepSimple(
    const std::vector<Family<int>> &family_flows, const TreeNodeEx &root,
    TreeNodeEx &node) {
  for (auto output_index = 0;
       output_index < static_cast<int>(node.outputs.size()); ++output_index) {
    const auto output = node.outputs[output_index];

    if (output < data_.output_range.min) {
      continue;
    }

    if (visited_outputs.contains(output)) {
      continue;
    }

    visited_outputs.emplace(output);

    if (InRange(output, data_.output_range)) {
      // can here be any issues? like when ouput->client is ok and
      // ouput->child->client is also ok
      output_tree_per_num_clients[output].emplace(1, client_node_);
    }

    // here we should chose how to traverse families.
    // we might sort them by the cost and then sort by other properties like
    // amount of pins

    for (const auto &family : family_flows) {
      auto &child =
          node.EmplaceChild(output_index, TreeNodeEx::FromFamily(family));

      RememberAlgStepSimple(family_flows, root, child);
    }
  }

  const auto first_output = node.outputs.front();
  const auto &output_combinations = output_tree_per_num_clients[first_output];
  const auto same_outputs = std::all_of(
      node.outputs.begin(), node.outputs.end(),
      [first_output](const auto output) { return output == first_output; });

  auto combination = std::map<int, std::pair<int, TreeNodeEx>>{};
  MakeCombination(node, combination, 0, output_combinations.size(), 0, 0,
                  same_outputs);
}
}  // namespace vh::ponc::flow