#include "calc_calculator.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <limits>
#include <set>
#include <stack>
#include <unordered_set>
#include <utility>
#include <vector>

#include "calc_resolution.h"
#include "calc_tree_node.h"
#include "cpp_assert.h"

namespace vh::ponc::calc {
///
Calculator::Calculator(const ConstructorArgs &args)
    : min_output_{ToCalculatorResolution(args.settings.min_output)},
      max_output_{ToCalculatorResolution(args.settings.max_output)},
      num_clients_{args.settings.num_clients},
      input_node_{args.input_node},
      client_node_{args.client_node},
      family_nodes_{args.family_nodes},
      step_callback_{args.step_callback} {
  input_node_.SetInput(std::numeric_limits<FlowValue>::max());

  std::stable_sort(
      family_nodes_.begin(), family_nodes_.end(),
      [](const auto &left, const auto &right) {
        return std::pair{left.GetNodeCost(), left.GetOutputs().size()} <
               std::pair{right.GetNodeCost(), right.GetOutputs().size()};
      });

  client_node_.SetNumClients(1);
  TraverseNode(input_node_);
}

///
auto Calculator::GetProgress() const -> float { return 0.5F; }

///
auto Calculator::GetResult() const -> const TreeNode & {
  const auto best_trees =
      best_tree_per_num_clients_per_output_.find(input_node_.GetInput());

  if (best_trees == best_tree_per_num_clients_per_output_.cend()) {
    return input_node_;
  }

  Expects(!best_trees->second.empty());
  const auto &biggest_tree = std::prev(best_trees->second.cend())->second;

  return biggest_tree;
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
// NOLINTNEXTLINE(*-no-recursion)
void Calculator::TraverseNode(TreeNode &node) {
  if (IsStopped()) {
    return;
  }

  const auto &outputs = node.GetOutputs();

  for (auto output_index = 0;
       output_index < static_cast<OutputIndex>(outputs.size());
       ++output_index) {
    if (IsStopped()) {
      return;
    }

    VisitOutput(node, output_index);
  }

  FindBestChildPermutationForNode(node);
}

///
// NOLINTNEXTLINE(*-no-recursion)
void Calculator::VisitOutput(TreeNode &node, OutputIndex output_index) {
  if (IsStopped()) {
    return;
  }

  const auto &outputs = node.GetOutputs();
  const auto output = outputs[output_index];

  if (output < min_output_) {
    return;
  }

  if (visited_outputs_.contains(output)) {
    return;
  }

  visited_outputs_.emplace(output);

  if (IsOutputInRange(output)) {
    best_tree_per_num_clients_per_output_.emplace(
        output, std::map{std::pair{1, client_node_}});
  }

  for (const auto &family_node : family_nodes_) {
    if (IsStopped()) {
      return;
    }

    auto &child = node.EmplaceChild(output_index, family_node);
    TraverseNode(child);
  }
}

///
void Calculator::FindBestChildPermutationForNode(const TreeNode &node) {
  if (IsStopped()) {
    return;
  }

  const auto &outputs = node.GetOutputs();
  const auto first_greater_than_min_ouput =
      std::find_if(outputs.cbegin(), outputs.cend(),
                   [min_ouput = min_output_](const auto output) {
                     return output >= min_ouput;
                   });

  if (first_greater_than_min_ouput == outputs.end()) {
    return;
  }

  const auto first_output = *first_greater_than_min_ouput;

  auto state = RecursionState{
      .node = node,
      .node_has_same_valid_outputs = std::all_of(
          outputs.cbegin(), outputs.cend(),
          [first_output, min_output = min_output_](const auto output) {
            return (output < min_output) || (output == first_output);
          })};
  state.node.ClearChildren();

  Expects(best_tree_per_num_clients_per_output_.contains(first_output));

  const auto first_output_index = static_cast<int>(
      std::distance(outputs.begin(), first_greater_than_min_ouput));
  const auto num_ouput_trees = static_cast<int>(
      best_tree_per_num_clients_per_output_.at(first_output).size());

  MakeNextChildPermutation(state, first_output_index, num_ouput_trees);
}

///
// NOLINTNEXTLINE(*-no-recursion)
void Calculator::MakeNextChildPermutation(RecursionState &state,
                                          OutputIndex output_index,
                                          TreeIndex prev_output_tree_index) {
  if (IsStopped()) {
    return;
  }

  const auto &outputs = state.node.GetOutputs();
  const auto num_outputs = static_cast<int>(outputs.size());

  Expects(output_index >= 0);
  Expects(output_index < num_outputs);
  const auto output = outputs[output_index];

  const auto &best_output_trees =
      best_tree_per_num_clients_per_output_.find(output);

  auto tree_index = 0;

  if (best_output_trees != best_tree_per_num_clients_per_output_.end()) {
    tree_index = state.node_has_same_valid_outputs
                     ? prev_output_tree_index
                     : static_cast<int>(best_output_trees->second.size());
  }

  for (; tree_index >= 0; --tree_index) {
    if (IsStopped()) {
      return;
    }

    if (tree_index == 0) {
      state.node.EraseChild(output_index);
    } else {
      const auto &tree =
          std::next(best_output_trees->second.cbegin(), tree_index - 1)->second;
      state.node.EmplaceChild(output_index, tree);
    }

    const auto next_output_index = output_index + 1;

    if (next_output_index < num_outputs) {
      MakeNextChildPermutation(state, next_output_index, tree_index);
      continue;
    }

    TestChildPermutation(state.node);
  }
}

///
void Calculator::TestChildPermutation(const TreeNode &node) {
  if (IsStopped()) {
    return;
  }

  const auto num_clients = node.GetNumClients();

  if ((num_clients <= 0) || (num_clients > num_clients_)) {
    return;
  }

  const auto best_input_trees =
      best_tree_per_num_clients_per_output_.find(node.GetInput());

  if (best_input_trees == best_tree_per_num_clients_per_output_.cend()) {
    RecordChildPermutation(node);
    return;
  }

  if (!best_input_trees->second.contains(num_clients)) {
    RecordChildPermutation(node);
    return;
  }

  const auto &best_tree = best_input_trees->second.at(num_clients);
  const auto best_tree_cost = best_tree.GetTreeCost();
  const auto new_tree_cost = node.GetTreeCost();

  if (new_tree_cost > best_tree_cost) {
    return;
  }

  RecordChildPermutation(node);
}

///
void Calculator::RecordChildPermutation(const TreeNode &node) {
  if (IsStopped()) {
    return;
  }

  best_tree_per_num_clients_per_output_[node.GetInput()][node.GetNumClients()] =
      node;
}
}  // namespace vh::ponc::calc