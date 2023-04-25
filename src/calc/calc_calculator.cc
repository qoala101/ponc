#include "calc_calculator.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iterator>
#include <limits>
#include <set>
#include <stack>
#include <unordered_set>
#include <utility>
#include <vector>

#include "calc_resolution.h"
#include "calc_tree_node.h"
#include "core_i_family.h"
#include "cpp_assert.h"
#include "imgui_node_editor.h"

namespace vh::ponc::calc {
namespace {
///
auto FamilyNodesFrom(const Calculator::ConstructorArgs &args) {
  auto family_nodes = std::vector<TreeNode>{};

  for (const auto &settings : args.settings.family_settings) {
    if (!settings.enabled) {
      continue;
    }

    const auto family_id = settings.family_id.Get();
    Expects(args.family_outputs.contains(family_id));

    const auto &outputs = args.family_outputs.at(family_id);
    family_nodes.emplace_back(settings.family_id, outputs, settings.cost);
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

///
Calculator::Calculator(const ConstructorArgs &args)
    : min_output_{ToCalculatorResolution(args.settings.min_output)},
      max_output_{ToCalculatorResolution(args.settings.max_output)},
      num_clients_{args.settings.num_clients},
      input_trees_{args.input_trees},
      client_node_{args.client_node},
      family_nodes_{FamilyNodesFrom(args)},
      step_callback_{args.step_callback} {
  client_node_.SetNumClients(1);
  VisitOutput(input_trees_[0], 0);
}

///
auto Calculator::GetProgress() const -> float {
  return static_cast<float>(GetLastResult().GetInput() - min_output_) /
         static_cast<float>(input_trees_[0].GetOutputs()[0] - min_output_);
}

///
auto Calculator::GetLastResult() const -> const TreeNode & {
  if (best_tree_per_num_clients_per_output_.empty()) {
    return input_trees_[0];
  }

  const auto &biggest_output_trees =
      std::prev(best_tree_per_num_clients_per_output_.cend())->second;
  Expects(!biggest_output_trees.empty());

  const auto &most_clients_tree =
      std::prev(biggest_output_trees.cend())->second;
  return most_clients_tree;
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