#include "calc_calculator.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
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
       output_index < static_cast<int>(permutation.size()); ++output_index) {
    const auto &best_output_tree = permutation[output_index];

    if (!best_output_tree.has_value()) {
      continue;
    }

    permutation_tree.tree_cost_ += (*best_output_tree)->tree_cost_;
    permutation_tree.num_clients_ += (*best_output_tree)->num_clients_;
    permutation_tree.child_nodes_.emplace(output_index, **best_output_tree);
  }

  return permutation_tree;
}
}  // namespace

Calculator::Calculator(const ConstructorArgs &args)
    : min_output_{ToCalculatorResolution(args.settings.min_output)},
      max_output_{ToCalculatorResolution(args.settings.max_output)},
      num_clients_{args.settings.num_clients},
      input_nodes_{args.input_nodes},
      client_node_{args.client_node},
      family_nodes_{args.family_nodes},
      step_callback_{args.step_callback} {
  client_node_.num_clients_ = 1;

  std::stable_sort(family_nodes_.begin(), family_nodes_.end(),
                   [](const auto &left, const auto &right) {
                     return std::pair{left.node_cost_, left.outputs_.size()} <
                            std::pair{right.node_cost_, right.outputs_.size()};
                   });

  FindUniqueOutputs();
  FindBestTrees();

  root_input_ = 1000000;

  auto next_node_output = root_input_ - 1;

  for (const auto &input_node : input_nodes_) {
    root_.outputs_.emplace_back(next_node_output - root_input_);

    auto input_node_family = input_node;

    for (auto &output : input_node_family.outputs_) {
      output -= next_node_output;
    }

    FindBestTreesForOutput(next_node_output, input_node_family);

    --next_node_output;
  }

  FindBestTreesForOutput(root_input_, root_);
}

auto Calculator::GetProgress() const -> float {
  // TODO(vh): Use latest best_trees_ entry.
  return 0.5F;
}

auto Calculator::TakeResult() -> std::vector<TreeNode> {
  // input_nodes_[0].child_nodes_.emplace(0, family_nodes_[0]);
  // input_nodes_[1].child_nodes_.emplace(1, client_node_);
  // input_nodes_[2].child_nodes_.emplace(3, client_node_);
  // return input_nodes_;

  const auto best_output_trees = best_trees_.find(root_input_);

  if (best_output_trees == best_trees_.end()) {
    return {};
  }

  Expects(!best_output_trees->second.empty());
  const auto best_output_tree = std::prev(best_output_trees->second.end());

  Expects(best_output_tree->second.outputs_.size() == input_nodes_.size());
  auto calculated_trees = std::vector<TreeNode>{};

  for (auto i = 0; i < input_nodes_.size(); ++i) {
    const auto calculated_tree = best_output_tree->second.child_nodes_.find(i);

    if (calculated_tree == best_output_tree->second.child_nodes_.cend()) {
      calculated_trees.emplace_back(std::move(input_nodes_[i]));
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
    for (const auto output : input_node.outputs_) {
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
        for (const auto family_output : family_node.outputs_) {
          const auto output_sum = output + family_output;

          if (output_sum >= min_output_) {
            unique_outputs_.emplace(output_sum);
          }
        }
      }

      visited_outputs.emplace(output);
    }
  }

  std::cout << "unique outputs: ";
  for (const auto ouput : unique_outputs_) {
    std::cout << ouput << ", ";
  }
  std::cout << "\n";
}

///
void Calculator::FindBestTrees() {
  for (const auto output : unique_outputs_) {
    std::cout << "working on output: " << output << "\n";

    if (IsOutputInRange(output)) {
      best_trees_.emplace(output, std::map{std::pair{1, client_node_}});
    }

    for (const auto &family_node : family_nodes_) {
      FindBestTreesForOutput(output, family_node);
    }
  }

  std::cout << "done\n";
}

///
void Calculator::FindBestTreesForOutput(FlowValue output,
                                        const TreeNode &family_node) {
  auto permutation =
      std::vector<std::optional<const TreeNode *>>(family_node.outputs_.size());
  auto num_clients_indices = std::unordered_map<FlowValue, NumClientsIndex>{};

  for (const auto family_output : family_node.outputs_) {
    const auto output_sum = output + family_output;
    num_clients_indices.emplace(output_sum, 0);
  }

  MakeBestTreesPermutation(output, family_node, permutation, 0,
                           num_clients_indices);
}

///
// NOLINTNEXTLINE(*-no-recursion)
void Calculator::MakeBestTreesPermutation(
    FlowValue output, const TreeNode &family_node,
    std::vector<std::optional<const TreeNode *>> &permutation,
    FlowValue output_index,
    std::unordered_map<FlowValue, NumClientsIndex> num_clients_indices) {
  auto permutation_num_clients = 0;
  auto permutation_tree_cost = family_node.node_cost_;

  for (const auto &best_output_tree : permutation) {
    if (!best_output_tree.has_value()) {
      continue;
    }

    permutation_num_clients += (*best_output_tree)->num_clients_;
    permutation_tree_cost += (*best_output_tree)->tree_cost_;
  }

  if (permutation_num_clients > num_clients_) {
    return;
  }

  const auto best_existing_tree = GetBestTree(output, permutation_num_clients);

  if (best_existing_tree.has_value() &&
      permutation_tree_cost > (*best_existing_tree)->tree_cost_) {
    return;
  }

  if (const auto permutation_is_ready =
          output_index >= static_cast<int>(family_node.outputs_.size())) {
    if (permutation_num_clients <= 0) {
      return;
    }

    if (!best_existing_tree.has_value()) {
      best_trees_[output][permutation_num_clients] =
          MakePermutationTree(family_node, permutation);
      return;
    }

    if (permutation_tree_cost < (*best_existing_tree)->tree_cost_) {
      **best_existing_tree = MakePermutationTree(family_node, permutation);
      return;
    }

    // TODO(vh): Compare equal cost trees.
    return;
  }

  const auto next_ouput_index = output_index + 1;

  Expects(output_index >= 0);
  const auto output_sum = output + family_node.outputs_[output_index];
  const auto best_output_trees = best_trees_.find(output_sum);

  if (best_output_trees != best_trees_.end()) {
    const auto num_clients_index = num_clients_indices.find(output_sum);
    Expects(num_clients_index != num_clients_indices.end());

    auto best_output_tree = best_output_trees->second.crbegin();
    std::advance(best_output_tree, num_clients_index->second);

    // NOLINTNEXTLINE(*-loop-convert)
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

  if (best_output_trees == best_trees_.end()) {
    return std::nullopt;
  }

  const auto best_num_clients_tree =
      best_output_trees->second.find(num_clients);

  if (best_num_clients_tree == best_output_trees->second.end()) {
    return std::nullopt;
  }

  return &best_num_clients_tree->second;
}
}  // namespace vh::ponc::calc