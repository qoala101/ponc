/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "calc_calculator.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iterator>
#include <limits>
#include <numeric>
#include <optional>
#include <set>
#include <stack>
#include <unordered_set>
#include <utility>
#include <vector>

#include "calc_resolution.h"
#include "calc_tree_node.h"
#include "cpp_assert.h"

namespace vh::ponc::calc {
namespace {
///
auto MakePermutationTree(
    const TreeNode &family_node,
    const std::vector<std::optional<const TreeNode *>> &permutation) {
  auto permutation_tree = family_node;

  for (auto output_index = 0;
       output_index < static_cast<OutputIndex>(permutation.size());
       ++output_index) {
    const auto &best_output_tree = permutation[output_index];

    if (!best_output_tree.has_value()) {
      continue;
    }

    permutation_tree.tree_cost += (*best_output_tree)->tree_cost;
    permutation_tree.num_clients += (*best_output_tree)->num_clients;
    permutation_tree.child_nodes.emplace(output_index, **best_output_tree);
  }

  return permutation_tree;
}

///
constexpr auto kRootInput = std::numeric_limits<FlowValue>::max();
}  // namespace

///
Calculator::Calculator(const ConstructorArgs &args)
    : min_output_{ToCalculatorResolution(args.settings.min_output)},
      max_output_{ToCalculatorResolution(args.settings.max_output)},
      num_clients_{args.settings.num_clients},
      input_nodes_{args.input_nodes},
      client_node_{args.client_node},
      family_nodes_{args.family_nodes},
      step_callback_{args.step_callback} {
  std::stable_sort(family_nodes_.begin(), family_nodes_.end(),
                   [](const auto &left, const auto &right) {
                     return std::pair{left.node_cost, left.outputs.size()} <
                            std::pair{right.node_cost, right.outputs.size()};
                   });

  FindUniqueOutputs();
  FindBestOutputTrees();
  FindBestRootTree();
}

///
auto Calculator::GetProgress() const -> float {
  if (unique_outputs_.empty() || best_trees_.empty()) {
    return 0;
  }

  const auto min_output = *unique_outputs_.cbegin();
  const auto max_output = *std::prev(unique_outputs_.cend());
  const auto working_on_output =
      std::clamp(std::prev(best_trees_.cend())->first, min_output, max_output);

  return static_cast<float>(working_on_output - min_output) /
         static_cast<float>(max_output - min_output);
}

///
auto Calculator::TakeResult() -> std::vector<TreeNode> {
  const auto best_output_trees = best_trees_.find(kRootInput);

  if (best_output_trees == best_trees_.cend()) {
    return std::move(input_nodes_);
  }

  Expects(!best_output_trees->second.empty());
  const auto best_output_tree = std::prev(best_output_trees->second.cend());

  Expects(best_output_tree->second.outputs.size() == input_nodes_.size());

  auto calculated_trees = std::vector<TreeNode>{};
  calculated_trees.reserve(input_nodes_.size());

  for (auto output_index = 0;
       output_index < static_cast<OutputIndex>(input_nodes_.size());
       ++output_index) {
    const auto calculated_tree =
        best_output_tree->second.child_nodes.find(output_index);

    if (calculated_tree == best_output_tree->second.child_nodes.cend()) {
      calculated_trees.emplace_back(std::move(input_nodes_[output_index]));
      continue;
    }

    calculated_trees.emplace_back(std::move(calculated_tree->second));
  }

  return calculated_trees;
}

///
auto Calculator::IsOutputInRange(FlowValue ouput) const {
  return (ouput >= min_output_) && (ouput <= max_output_);
}

///
auto Calculator::IsStopped() {
  return step_callback_(*this) == StepStatus::kStopCalculation;
}

///
void Calculator::FindUniqueOutputs() {
  for (const auto &input_node : input_nodes_) {
    for (const auto output : input_node.outputs) {
      unique_outputs_.emplace(output);
    }
  }

  auto visited_outputs = std::unordered_set<FlowValue>{};

  while (visited_outputs.size() < unique_outputs_.size()) {
    for (const auto output : unique_outputs_) {
      if (visited_outputs.contains(output)) {
        continue;
      }

      for (const auto &family_node : family_nodes_) {
        for (const auto family_output : family_node.outputs) {
          const auto output_sum = output + family_output;

          if (output_sum >= min_output_) {
            unique_outputs_.emplace(output_sum);
          }
        }
      }

      visited_outputs.emplace(output);
    }
  }
}

///
void Calculator::FindBestOutputTrees() {
  for (const auto output : unique_outputs_) {
    if (IsStopped()) {
      return;
    }

    if (IsOutputInRange(output)) {
      best_trees_.emplace(
          output, std::map{std::pair{client_node_.num_clients, client_node_}});
    }

    for (const auto &family_node : family_nodes_) {
      FindBestTreesForOutput(output, family_node);
    }
  }
}

///
void Calculator::FindBestTreesForOutput(FlowValue output,
                                        const TreeNode &family_node) {
  auto permutation =
      std::vector<std::optional<const TreeNode *>>(family_node.outputs.size());
  auto num_clients_indices = std::unordered_map<FlowValue, NumClientsIndex>{};

  for (const auto family_output : family_node.outputs) {
    const auto output_sum = output + family_output;
    num_clients_indices.emplace(output_sum, 0);
  }

  MakeBestTreesPermutation(output, family_node, permutation, 0,
                           num_clients_indices);
}

///
auto Calculator::TestBestTreesPermutation(
    FlowValue output, const TreeNode &family_node,
    std::vector<std::optional<const TreeNode *>> &permutation,
    FlowValue output_index) {
  auto permutation_num_clients = 0;
  auto permutation_tree_cost = family_node.node_cost;

  for (const auto &best_output_tree : permutation) {
    if (!best_output_tree.has_value()) {
      continue;
    }

    permutation_num_clients += (*best_output_tree)->num_clients;
    permutation_tree_cost += (*best_output_tree)->tree_cost;
  }

  if (permutation_num_clients > num_clients_) {
    return false;
  }

  const auto best_existing_tree = GetBestTree(output, permutation_num_clients);

  if (best_existing_tree.has_value() &&
      permutation_tree_cost > (*best_existing_tree)->tree_cost) {
    return false;
  }

  if (const auto permutation_is_ready =
          output_index >=
          static_cast<OutputIndex>(family_node.outputs.size())) {
    if (permutation_num_clients <= 0) {
      return false;
    }

    if (!best_existing_tree.has_value()) {
      best_trees_[output][permutation_num_clients] =
          MakePermutationTree(family_node, permutation);
      return false;
    }

    if (permutation_tree_cost < (*best_existing_tree)->tree_cost) {
      **best_existing_tree = MakePermutationTree(family_node, permutation);
    }

    // TODO(vh): Compare equal cost trees.
    return false;
  }

  return true;
}

///
// NOLINTNEXTLINE(*-no-recursion)
void Calculator::MakeBestTreesPermutation(
    FlowValue output, const TreeNode &family_node,
    std::vector<std::optional<const TreeNode *>> &permutation,
    FlowValue output_index,
    std::unordered_map<FlowValue, NumClientsIndex> num_clients_indices) {
  if (IsStopped()) {
    return;
  }

  if (!TestBestTreesPermutation(output, family_node, permutation,
                                output_index)) {
    return;
  }

  const auto next_ouput_index = output_index + 1;

  Expects(output_index >= 0);
  const auto output_sum = output + family_node.outputs[output_index];
  const auto best_output_trees = best_trees_.find(output_sum);

  if (best_output_trees != best_trees_.cend()) {
    const auto num_clients_index = num_clients_indices.find(output_sum);
    Expects(num_clients_index != num_clients_indices.cend());

    auto best_output_tree = best_output_trees->second.crbegin();
    std::advance(best_output_tree, num_clients_index->second);

    for (; best_output_tree != best_output_trees->second.crend();
         ++best_output_tree) {
      permutation[output_index] = &best_output_tree->second;
      MakeBestTreesPermutation(output, family_node, permutation,
                               next_ouput_index, num_clients_indices);

      ++num_clients_index->second;
    }
  }

  permutation[output_index] = std::nullopt;
  MakeBestTreesPermutation(output, family_node, permutation, next_ouput_index,
                           num_clients_indices);
}

///
auto Calculator::GetBestTree(FlowValue output, NumClients num_clients)
    -> std::optional<TreeNode *> {
  const auto best_output_trees = best_trees_.find(output);

  if (best_output_trees == best_trees_.cend()) {
    return std::nullopt;
  }

  const auto best_num_clients_tree =
      best_output_trees->second.find(num_clients);

  if (best_num_clients_tree == best_output_trees->second.cend()) {
    return std::nullopt;
  }

  return &best_num_clients_tree->second;
}

///
void Calculator::FindBestRootTree() {
  auto root_family = TreeNode{};
  root_family.outputs.reserve(input_nodes_.size());

  auto next_node_input = kRootInput - 1;

  for (const auto &input_node : input_nodes_) {
    root_family.outputs.emplace_back(next_node_input - kRootInput);

    auto input_node_family = input_node;

    for (auto &output : input_node_family.outputs) {
      output -= next_node_input;
    }

    FindBestTreesForOutput(next_node_input, input_node_family);
    --next_node_input;
  }

  FindBestTreesForOutput(kRootInput, root_family);
}
}  // namespace vh::ponc::calc