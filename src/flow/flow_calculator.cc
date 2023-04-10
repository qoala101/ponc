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

auto InRange(int value, const InputRange &range) {
  return (value >= range.min) && (value <= range.max);
}

auto GetMinOutput(const std::vector<InputRange> &output_ranges) {
  auto min_output = std::numeric_limits<int>::max();

  for (const auto &range : output_ranges) {
    min_output = std::min(range.min, min_output);
  }

  return min_output;
}

struct RunData {
  int I = 0;
  int DEPTH = 0;
  int NUM_CHECKED = 0;
  std::vector<InputRange> sorted_output_ranges{};
  std::vector<TreeNodeEx> out_trees{};
  int min_output{};
  TreeNodeEx client{};

  InputRange RANGE{};
  int NUM_CLIENTS{};

  std::set<int> visited_outputs{};
  std::stack<int> UNIQUE_STACK{};
  std::map<int, int> OUTPUT_CHILDREN{};
  std::map<int, std::set<uintptr_t>> OUPUT_FAMILIES{};

  std::map<int, std::map<int, TreeNodeEx>> output_tree_per_num_clients{};
};

auto RUN = RunData{};

auto Cout() -> std::ostream & {
  struct S : public std::ostream {};
  static auto s = S{};
  return std::cout;
  return s;
}

void CheckUniqueness(const std::vector<TreeNodeEx> &out_trees) {
  std::cout << "Checking for duplicates in " << out_trees.size()
            << " trees...\n";

  auto num_bad = 0;

  for (auto i = 0; i < out_trees.size(); ++i) {
    const auto &tree_i = out_trees[i];

    for (auto j = i; j < out_trees.size(); ++j) {
      if (j == i) {
        continue;
      }

      if (out_trees[j] == tree_i) {
        Cout() << "EQUAL BAD " << i << " " << j << "\n";
        ++num_bad;
      }
    }
  }

  if (num_bad > 0) {
    std::cout << "NUM BAD: " << num_bad << "\n";
  } else {
    std::cout << "No duplicates\n";
  }
}

void MAKE_TEST_COMBINATION(
    const TreeNodeEx &node,
    std::map<int /*ouput_index*/, std::pair<int, TreeNodeEx>> &combination,
    int output_index) {
  if (output_index < node.outputs.size()) {
    const auto output = node.outputs[output_index];
    const auto &output_combinations = RUN.output_tree_per_num_clients[output];

    for (auto output_combination_index = 0;
         output_combination_index <= output_combinations.size();
         ++output_combination_index) {
      if (output_combination_index == 0) {
        combination.erase(output_index);
      } else {
        combination[output_index] = *std::next(output_combinations.begin(),
                                               output_combination_index - 1);
      }

      MAKE_TEST_COMBINATION(node, combination, output_index + 1);
    }

    return;
  }

  // its just printing
  // for (auto output_index = 0; output_index < node.outputs.size();
  //      ++output_index) {
  //   std::cout << node.outputs[output_index] << ": ";

  //   if (combination.contains(output_index)) {
  //     std::cout << combination.at(output_index).first << " "
  //               << combination.at(output_index).second.CalculateCost();
  //   } else {
  //     std::cout << 0;
  //   }

  //   std::cout << "\n";
  // }

  // std::cout << "\n";

  auto clients_sum = 0;
  auto total_cost = node.cost;

  for (auto output_index = 0; output_index < node.outputs.size();
       ++output_index) {
    if (!combination.contains(output_index)) {
      continue;
    }

    const auto &[num_clients, tree] = combination.at(output_index);
    clients_sum += num_clients;
    total_cost += tree.CalculateCost();
  }

  if ((clients_sum < 1) || (clients_sum > RUN.NUM_CLIENTS)) {
    return;
  }

  auto &input_client_variants = RUN.output_tree_per_num_clients[node.input];

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

void SAME_OUTPUTS(const TreeNodeEx &node) {
  auto &input_client_variants = RUN.output_tree_per_num_clients[node.input];
  const auto first_output = node.outputs.front();
  const auto num_outputs = node.outputs.size();
  const auto &tree_per_num_clients =
      RUN.output_tree_per_num_clients[first_output];

  if (tree_per_num_clients.empty()) {
    return;
  }

  // auto tree_per_num_clients_sorted_by_avg_cost =
  //     std::vector<std::pair<int, TreeNodeEx>>{};
  // std::transform(tree_per_num_clients.begin(), tree_per_num_clients.end(),
  //                std::back_inserter(tree_per_num_clients_sorted_by_avg_cost),
  //                [](const auto &pair) { return pair; });
  // std::sort(tree_per_num_clients_sorted_by_avg_cost.begin(),
  //           tree_per_num_clients_sorted_by_avg_cost.end(),
  //           [](const auto &left, const auto &right) {
  //             const auto left_avg_cost =
  //                 static_cast<float>(left.second.CalculateCost()) /
  //                 left.first;
  //             const auto right_avg_cost =
  //                 static_cast<float>(right.second.CalculateCost()) /
  //                 right.first;
  //             return left_avg_cost < right_avg_cost;
  //           });

  if (node.input == 170) {
    std::cout << "Num outputs: " << num_outputs << "\n";

    for (const auto &pair : tree_per_num_clients) {
      std::cout << pair.first << " " << pair.second.CalculateCost() << "\n";
    }
  }

  for (auto num_clients = 1; num_clients <= RUN.NUM_CLIENTS; ++num_clients) {
    auto node_copy = node;
    node_copy.child_nodes.clear();

    auto clients_sum = 0;
    auto ouput_index = 0;
    auto total_cost = node.cost;

    while ((clients_sum < num_clients) && (ouput_index < num_outputs)) {
      const auto need_more_clients = num_clients - clients_sum;

      if (tree_per_num_clients.contains(need_more_clients)) {
        const auto &tree = tree_per_num_clients.at(need_more_clients);
        node_copy.EmplaceChild(ouput_index, tree);
        clients_sum += need_more_clients;
        total_cost += tree.CalculateCost();
      } else {
        const auto biggest = std::prev(tree_per_num_clients.end());
        const auto &tree = biggest->second;
        node_copy.EmplaceChild(ouput_index, biggest->second);
        clients_sum += biggest->first;
        total_cost += tree.CalculateCost();
      }

      ++ouput_index;
    }

    if (input_client_variants.contains(clients_sum) &&
        input_client_variants.at(clients_sum).CalculateCost() <= total_cost) {
      continue;
    }

    input_client_variants[clients_sum] = std::move(node_copy);
  }
}

void RememberAlgStepSimple(const std::vector<FamilyFlow> &family_flows,
                           const TreeNodeEx &root, TreeNodeEx &node) {
  for (auto output_index = 0; output_index < node.outputs.size();
       ++output_index) {
    const auto output = node.outputs[output_index];

    if (output < RUN.min_output) {
      continue;
    }

    if (RUN.visited_outputs.contains(output)) {
      continue;
    }

    RUN.visited_outputs.emplace(output);

    if (InRange(output, RUN.RANGE)) {
      RUN.output_tree_per_num_clients[output].emplace(1, RUN.client);
    }

    for (auto family_i = 0; family_i < family_flows.size(); ++family_i) {
      auto &child =
          node.EmplaceChild(output_index, TreeNodeEx{family_flows[family_i]});

      ++RUN.DEPTH;
      RememberAlgStepSimple(family_flows, root, child);
      --RUN.DEPTH;
    }
  }

  const auto first_output = node.outputs.front();

  if (std::all_of(node.outputs.begin(), node.outputs.end(),
                  [first_output](const auto output) {
                    return output == first_output;
                  })) {
    SAME_OUTPUTS(node);
    return;
  }

  auto combination = std::map<int, std::pair<int, TreeNodeEx>>{};
  MAKE_TEST_COMBINATION(node, combination, 0);
}

auto RememberAlg(TreeNodeEx root, TreeNodeEx client,
                 const std::vector<FamilyFlow> &family_flows,
                 const std::vector<InputRange> &output_ranges) {
  RUN = RunData{};
  RUN.sorted_output_ranges = output_ranges;
  RUN.out_trees = std::vector<TreeNodeEx>{};
  RUN.min_output = GetMinOutput(output_ranges);
  RUN.client = std::move(client);
  RUN.RANGE = RUN.sorted_output_ranges.front();
  RUN.NUM_CLIENTS = RUN.sorted_output_ranges.size();

  RememberAlgStepSimple(family_flows, root, root);

  std::cout << "unique_outputs: ";
  for (const auto input : RUN.visited_outputs) {
    std::cout << input << ", ";
  }
  std::cout << "\n";

  Expects(!root.outputs.empty());
  auto &input_client_variants =
      RUN.output_tree_per_num_clients[root.outputs.front()];

  for (const auto &[i, tree] : input_client_variants) {
    auto &root_copy = RUN.out_trees.emplace_back(root);
    root_copy.child_nodes[0] = tree;
  }

  return RUN.out_trees;
}
}  // namespace

TreeNodeEx::TreeNodeEx(const FamilyFlow &family_flow)
    : family_id{family_flow.family_id},
      outputs{family_flow.outputs},
      cost{family_flow.cost} {}

auto TreeNodeEx::GetNumChildren() const -> int {
  return static_cast<int>(child_nodes.size());
}

auto TreeNodeEx::GetNumOutputs() const -> int {
  return static_cast<int>(outputs.size());
}

auto TreeNodeEx::GetChildIndex(const TreeNodeEx *child) const -> int {
  const auto found_child =
      std::find_if(child_nodes.begin(), child_nodes.end(),
                   [child](const auto &pair) { return &pair.second == child; });
  Expects(found_child != child_nodes.end());
  return found_child->first;
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
  child.parent = this;

  for (auto &output : child.outputs) {
    output += child.input;
  }

  return child_nodes[index] = std::move(child);
}

auto TreeNodeEx::AreOutputsLessThan(int value) const -> bool {
  return std::all_of(outputs.begin(), outputs.end(),
                     [value](const auto output) { return output < value; });
}

auto FamilyFlow::GetMinDecrementOutput() const -> int {
  auto min_output = std::numeric_limits<int>::lowest();

  for (const auto output : outputs) {
    Expects(output < 0);
    min_output = std::max(output, min_output);
  }

  return min_output;
}

auto FamilyFlow::GetMaxDecrementOutput() const -> int {
  auto max_output = std::numeric_limits<int>::max();

  for (const auto output : outputs) {
    Expects(output < 0);
    max_output = std::min(output, max_output);
  }

  return max_output;
}

auto Calculate(const CalculatorInput &input) -> std::vector<TreeNodeEx> {
  // return TEST(input.root, input.client, input.family_flows,
  // input.input_ranges);
  return RememberAlg(input.root, input.client, input.family_flows,
                     input.input_ranges);
}
}  // namespace vh::ponc::flow