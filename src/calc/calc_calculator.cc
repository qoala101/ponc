#include "calc_calculator.h"

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

#include "calc_tree_node.h"
#include "core_i_family.h"
#include "core_id_generator.h"
#include "cpp_assert.h"
#include "cpp_scope.h"
#include "flow_tree.h"
#include "flow_tree_traversal.h"
#include "imgui_node_editor.h"

namespace vh::ponc::calc {
namespace {
///
auto ToInt(float value) { return static_cast<int>(value * 100); }

///
auto FamilyNodesFrom(const Calculator::ConstructorArgs &args) {
  auto family_nodes = std::vector<TreeNode>{};

  for (const auto &settings : args.settings.family_settings) {
    if (!settings.enabled) {
      continue;
    }

    const auto family_id = settings.family_id.Get();

    Expects(args.family_outputs.contains(family_id));
    const auto &float_outputs = args.family_outputs.at(family_id);

    auto outputs = std::vector<FlowValue>{};
    outputs.reserve(float_outputs.size());

    std::transform(float_outputs.begin(), float_outputs.end(),
                   std::back_inserter(outputs), &ToInt);

    family_nodes.emplace_back(settings.family_id, std::move(outputs),
                              settings.cost);
  }

  std::stable_sort(
      family_nodes.begin(), family_nodes.end(),
      [](const auto &left, const auto &right) {
        return std::pair{left.GetNodeCost(), left.GetOutputs().size()} <
               std::pair{right.GetNodeCost(), right.GetOutputs().size()};
      });

  return family_nodes;
}
}  // namespace

Calculator::Calculator(const ConstructorArgs &args)
    : min_output_{ToInt(args.settings.min_output)},
      max_output_{ToInt(args.settings.max_output)},
      num_clients_{args.settings.num_clients},
      input_node_{args.input_family_id, {ToInt(args.settings.input)}},
      client_node_{args.client_family_id},
      family_nodes_{FamilyNodesFrom(args)},
      step_callback_{args.step_callback} {
  TraverseNode(input_node_);

  std::cout << "unique_outputs: ";
  for (const auto input : visited_outputs) {
    std::cout << input << ", ";
  }
  std::cout << "\n";

  // if (!output_tree_per_num_clients.contains(data_.input)) {
  //   return;
  // }

  // // auto &input_client_variants =
  // output_tree_per_num_clients.at(data_.input);

  // auto &input_client_variants =
  //     std::prev(output_tree_per_num_clients.end())->second;

  // if (input_client_variants.empty()) {
  //   return;
  // }

  // auto &[num_clients, tree] = *std::prev(input_client_variants.end());
  // result_.EmplaceChild(0, std::move(tree));

  // progress_.READY = true;
  // step_callback_(*this);
}

auto Calculator::IsReady() const -> bool { return ready_; }

auto Calculator::GetProgress() const -> float { return progress_; }

auto Calculator::GetLastResult() const -> const TreeNode & {
  return input_node_;
}

///
auto Calculator::IsOutputInRange(FlowValue ouput) const {
  return (ouput >= min_output_) && (ouput <= max_output_);
}

void Calculator::MakeCombination(
    const TreeNode &node,
    std::map<OutputIndex, std::pair<NumClients, TreeNode>> &combination,
    OutputIndex output_index, int start_with_combination,
    NumClients clients_sum, Cost clients_cost, bool same_outputs) {
  const auto &outputs = node.GetOutputs();
  const auto output = outputs[output_index];
  const auto &output_combinations = cheapest_trees_per_output[output];
  const auto num_outputs = static_cast<int>(outputs.size());

  auto &input_client_variants = cheapest_trees_per_output[node.GetInput()];

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
      combination[output_index] =
          *std::next(output_combinations.begin(), output_combination_index - 1);
      clients_sum += combination[output_index].first;
      clients_cost += combination[output_index].second.GetTreeCost();
    }

    if (clients_sum > num_clients_) {
      continue;
    }

    if (input_client_variants.contains(clients_sum) &&
        (clients_cost > input_client_variants.at(clients_sum).GetTreeCost())) {
      continue;
    }

    const auto next_output_index = output_index + 1;

    if (next_output_index < num_outputs) {
      MakeCombination(node, combination, next_output_index,
                      output_combination_index, clients_sum, clients_cost,
                      same_outputs);
      continue;
    }

    TestCombination(node, combination, clients_sum, clients_cost, same_outputs);
  }
}

void Calculator::TestCombination(
    const TreeNode &node,
    const std::map<OutputIndex, std::pair<NumClients, TreeNode>> &combination,
    NumClients clients_sum, Cost clients_cost, bool same_outputs) {
  if (clients_sum <= 0) {
    return;
  }

  auto total_cost = node.GetNodeCost() + clients_cost;
  auto &input_client_variants = cheapest_trees_per_output[node.GetInput()];

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

  for (auto output_index = 0; output_index < node.GetOutputs().size();
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

///
// NOLINTNEXTLINE(*-no-recursion)
void Calculator::VisitOutput(TreeNode &node, OutputIndex output_index) {
  const auto &outputs = node.GetOutputs();
  const auto output = outputs[output_index];

  if (output < min_output_) {
    return;
  }

  if (visited_outputs.contains(output)) {
    return;
  }

  visited_outputs.emplace(output);

  if (IsOutputInRange(output)) {
    cheapest_trees_per_output.emplace(output,
                                      std::map{std::pair{1, client_node_}});
  }

  for (const auto &family_node : family_nodes_) {
    auto &child = node.EmplaceChild(output_index, family_node);
    TraverseNode(child);
  }
}

void Calculator::TraverseNode(TreeNode &node) {
  const auto &outputs = node.GetOutputs();

  for (auto output_index = 0;
       output_index < static_cast<OutputIndex>(outputs.size());
       ++output_index) {
    VisitOutput(node, output_index);
  }

  Expects(!outputs.empty());
  const auto first_output = outputs.front();
  const auto same_outputs = std::all_of(
      outputs.begin(), outputs.end(),
      [first_output](const auto output) { return output == first_output; });

  // Expects(cheapest_trees_per_output.contains(first_output));
  const auto &output_combinations = cheapest_trees_per_output[first_output];

  auto combination = std::map<FlowValue, std::pair<NumClients, TreeNode>>{};
  MakeCombination(node, combination, 0, output_combinations.size(), 0, 0,
                  same_outputs);
}
}  // namespace vh::ponc::calc