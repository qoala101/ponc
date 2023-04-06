#include "flow_calculator.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
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

auto InRange(float value, const InputRange &range) {
  return (value >= range.min) && (value <= range.max);
}

auto GetMinOutput(const std::vector<InputRange> &output_ranges) {
  auto min_output = std::numeric_limits<float>::max();

  for (const auto &range : output_ranges) {
    min_output = std::min(range.min, min_output);
  }

  return min_output;
}

auto GetMinDecrement(const std::vector<FamilyFlow> &family_flows) {
  auto min_output = std::numeric_limits<float>::lowest();

  for (const auto &family : family_flows) {
    min_output = std::max(family.GetMinDecrementOutput(), min_output);
  }

  return min_output;
}

auto GetFamilyIndex(core::FamilyId family_id,
                    const std::vector<FamilyFlow> &family_flows) {
  const auto family_flow =
      std::find_if(family_flows.begin(), family_flows.end(),
                   [family_id](const auto &family_flow) {
                     return family_flow.family_id == family_id;
                   });
  Expects(family_flow != family_flows.end());
  return static_cast<int>(std::distance(family_flows.begin(), family_flow));
}

class MinCostTrees {
 public:
  explicit MinCostTrees(int max_size) : max_size_{max_size} {}

  auto GetMaxCost() const {
    return trees_.empty() ? std::numeric_limits<float>::max()
                          : trees_.back().first;
  }

  auto IsFull() const { return static_cast<int>(trees_.size()) >= max_size_; }

  auto Emplace(float cost, int index) {
    trees_.emplace_back(cost, index);

    std::sort(trees_.begin(), trees_.end(),
              [](const auto &left, const auto &right) {
                return left.first < right.first;
              });

    if (static_cast<int>(trees_.size()) > max_size_) {
      trees_.resize(max_size_);
    }
  }

  std::vector<std::pair<float, int>> trees_{};

 private:
  int max_size_{};
};

struct RunData {
  int I = 0;
  int DEPTH = 0;
  int NUM_CHECKED = 0;
  std::vector<InputRange> sorted_output_ranges{};
  std::vector<TreeNodeEx> out_trees{};
  float min_output{};
  TreeNodeEx client{};
  MinCostTrees MIN_COST_TREES{10};

  std::set<float> unique_inputs{};
  std::stack<float> UNIQUE_STACK{};
  std::map<float, int> OUTPUT_CHILDREN{};
  std::map<float, std::set<uintptr_t>> OUPUT_FAMILIES{};
};

auto RUN = RunData{};

auto Cout() -> std::ostream & {
  struct S : public std::ostream {};
  static auto s = S{};
  return std::cout;
  return s;
}

auto ToString(const TreeNodeEx &tree) {
  auto text = std::string{};
  auto depth = 0;

  TraverseDepthFirstExConst(
      tree,
      [&text, &depth](const auto &node) {
        text += "[" + std::to_string(depth) + "] ";
        ++depth;
        for (auto i = 0; i < node.GetNumOutputs(); ++i) {
          if (node.child_nodes.contains(i)) {
            text += std::to_string(node.child_nodes.at(i).family_id.Get());
          } else {
            text += "0";
          }
        }

        text += " ";
      },
      [&depth](const auto &) { --depth; });

  return text;
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

auto GetNodeIndex(
    const std::vector<std::pair<TreeNodeEx *, int>> &current_level, int index)
    -> std::pair<TreeNodeEx *, int> {
  return current_level[index];
}

auto Contains(const std::vector<std::pair<TreeNodeEx *, int>> &current_level,
              int index) {
  const auto &[node, child_index] = GetNodeIndex(current_level, index);
  return node->child_nodes.contains(child_index);
}

auto At(const std::vector<std::pair<TreeNodeEx *, int>> &current_level,
        int index) -> auto & {
  const auto &[node, child_index] = GetNodeIndex(current_level, index);
  return node->child_nodes.at(child_index);
}

void Erase(const std::vector<std::pair<TreeNodeEx *, int>> &current_level,
           int index) {
  const auto &[node, child_index] = GetNodeIndex(current_level, index);
  node->child_nodes.erase(child_index);
}

auto EmplaceChild(
    const std::vector<std::pair<TreeNodeEx *, int>> &current_level, int index,
    TreeNodeEx new_node) -> auto & {
  const auto &[node, child_index] = GetNodeIndex(current_level, index);
  return node->EmplaceChild(child_index, std::move(new_node));
}

struct Group {
  std::vector<FamilyFlow> valid_families{};
  std::vector<std::pair<TreeNodeEx *, int>> nodes{};
};

auto MakeGroups(const std::vector<TreeNodeEx *> &level_nodes,
                const std::vector<FamilyFlow> &family_flows) {
  auto groups = std::map<float, Group, std::greater<>>{};

  for (auto *node : level_nodes) {
    auto index = 0;

    for (const auto output : node->outputs) {
      groups[output].nodes.emplace_back(node, index);
      ++index;
    }
  }

  for (auto &[output, group] : groups) {
    group.valid_families = family_flows;
  }

  return groups;
}

auto GetNextLevelNodes(
    const std::map<float, Group, std::greater<>> &current_level) {
  auto next_level_nodes = std::vector<TreeNodeEx *>{};

  for (const auto &group : current_level) {
    for (const auto &[node, index] : group.second.nodes) {
      if (node->child_nodes.contains(index)) {
        next_level_nodes.emplace_back(&node->child_nodes.at(index));
      }
    }
  }

  return next_level_nodes;
}

auto IsLevelEmpty(
    const std::map<float, Group, std::greater<>> &current_level_groups) {
  for (const auto &group : current_level_groups) {
    for (const auto &[node, index] : group.second.nodes) {
      if (!node->child_nodes.empty()) {
        return false;
      }
    }
  }

  return true;
}

struct Ouput {
  float value{};
  TreeNodeEx *tree_node{};
  int index{};
};

auto FindAndAddOutputs(TreeNodeEx &root) -> bool {
  auto sorted_free_outputs = std::vector<Ouput>{};

  TraverseDepthFirstEx(
      root,
      [&sorted_free_outputs](auto &tree_node) {
        auto index = 0;

        for (const auto output : tree_node.outputs) {
          if (!tree_node.child_nodes.contains(index)) {
            sorted_free_outputs.emplace_back(Ouput{
                .value = output, .tree_node = &tree_node, .index = index});
          }

          ++index;
        }
      },
      [](const auto &) {});

  std::stable_sort(sorted_free_outputs.begin(), sorted_free_outputs.end(),
                   [](const auto &left, const auto &right) {
                     return left.value < right.value;
                   });

  // VH: Here must be algorithm which would find a combination
  // of pins which match all required inputs. Currently I assume
  // that all ranges are the same so its enough to have the
  // amount of unique pins same as amount of ranges.
  const auto &FIRST_RANGE = RUN.sorted_output_ranges.front();
  const auto num_ranges = static_cast<int>(RUN.sorted_output_ranges.size());

  auto num_matches = 0;

  for (auto &output : sorted_free_outputs) {
    if (InRange(output.value, FIRST_RANGE)) {
      output.tree_node->EmplaceChild(output.index, RUN.client);
      ++num_matches;

      if (num_matches >= num_ranges) {
        return true;
      }
    }
  }

  return false;
}

void RecordTreeAndChooseGroupsToProceedDeeper(
    const TreeNodeEx &root,
    std::map<float, Group, std::greater<>> &next_level_groups) {
  ++RUN.NUM_CHECKED;

  // if (RUN.DEPTH > 1 || RUN.out_trees.size() > 100) {
  //   next_level_groups.clear();
  // }
  // Cout() << RUN.I++ << ": " << ToString(root) << "\n";
  // RUN.out_trees.emplace_back(root);
  // return;

  for (auto &[ouput, group] : next_level_groups) {
    auto valid_families = std::vector<FamilyFlow>{};

    for (auto &family : group.valid_families) {
      const auto output_would_be_ok =
          (ouput + family.GetMinDecrementOutput()) >= RUN.min_output;

      if (output_would_be_ok) {
        valid_families.emplace_back(std::move(family));
      }
    }

    group.valid_families = std::move(valid_families);
  }

  const auto cost = root.CalculateCost();
  const auto current_max_cost = RUN.MIN_COST_TREES.GetMaxCost();

  if ((cost >= current_max_cost) && RUN.MIN_COST_TREES.IsFull()) {
    next_level_groups.clear();
    return;
  }

  auto root_copy = root;

  if (FindAndAddOutputs(root_copy)) {
    Cout() << RUN.I++ << ": " << ToString(root_copy) << "\n";
    RUN.out_trees.emplace_back(std::move(root_copy));
    RUN.MIN_COST_TREES.Emplace(cost, RUN.out_trees.size() - 1);
  }
}

void CreateNextTransposition(
    const std::vector<FamilyFlow> &family_flows, const TreeNodeEx &root,
    const std::map<float, Group, std::greater<>> &current_level_groups,
    int output_index, int group_index) {
  // if (RUN.NUM_CHECKED > 1000) {
  //   return;
  // }
  // if (RUN.out_trees.size() > 182) {
  //   return;
  // }

  const auto num_groups = static_cast<int>(current_level_groups.size());
  const auto num_outputs_in_group =
      group_index < num_groups
          ? static_cast<int>(
                std::next(current_level_groups.begin(), group_index)
                    ->second.nodes.size())
          : 0;

  if (output_index < num_outputs_in_group) {
    const auto &current_group =
        std::next(current_level_groups.begin(), group_index)->second;

    auto prev_child_family_index = 0;

    if (output_index > 0) {
      const auto prev_output_index = output_index - 1;

      if (Contains(current_group.nodes, prev_output_index)) {
        prev_child_family_index =
            GetFamilyIndex(At(current_group.nodes, output_index - 1).family_id,
                           current_group.valid_families) +
            1;
      }
    }

    const auto num_families =
        static_cast<int>(current_group.valid_families.size());

    for (auto family_i = prev_child_family_index; family_i < num_families + 1;
         ++family_i) {
      if (family_i == 0) {
        Erase(current_group.nodes, output_index);
      } else {
        EmplaceChild(current_group.nodes, output_index,
                     TreeNodeEx{current_group.valid_families[family_i - 1]});
      }

      for (auto output_i = output_index + 1; output_i < num_outputs_in_group;
           ++output_i) {
        Erase(current_group.nodes, output_i);
      }

      CreateNextTransposition(family_flows, root, current_level_groups,
                              output_index + 1, group_index);
    }

    return;
  }

  if (group_index < num_groups) {
    return CreateNextTransposition(family_flows, root, current_level_groups, 0,
                                   group_index + 1);
  }

  const auto first_empty_transposition = IsLevelEmpty(current_level_groups);
  const auto next_level_nodes = GetNextLevelNodes(current_level_groups);
  auto next_level_groups = MakeGroups(next_level_nodes, family_flows);

  if (!first_empty_transposition) {
    for (auto *node : next_level_nodes) {
      node->child_nodes.clear();
    }

    RecordTreeAndChooseGroupsToProceedDeeper(root, next_level_groups);
  }

  if (next_level_groups.empty()) {
    return;
  }

  ++RUN.DEPTH;
  CreateNextTransposition(family_flows, root, next_level_groups, 0, 0);
  --RUN.DEPTH;
}

auto NewAlg(TreeNodeEx root, TreeNodeEx client,
            const std::vector<FamilyFlow> &family_flows,
            const std::vector<InputRange> &output_ranges) {
  RUN = RunData{};
  RUN.sorted_output_ranges = output_ranges;
  RUN.out_trees = std::vector<TreeNodeEx>{};
  RUN.min_output = GetMinOutput(output_ranges);
  RUN.client = std::move(client);

  std::sort(
      RUN.sorted_output_ranges.begin(), RUN.sorted_output_ranges.end(),
      [](const auto &left, const auto &right) { return left.min < right.min; });

  auto sorted_family_flows = family_flows;
  std::sort(sorted_family_flows.begin(), sorted_family_flows.end(),
            [](const auto &left, const auto &right) {
              return left.GetMaxDecrementOutput() <
                     right.GetMaxDecrementOutput();
            });

  std::cout << "Sorted families: ";
  for (const auto &family : sorted_family_flows) {
    std::cout << family.family_id.Get() << "(" << family.GetMaxDecrementOutput()
              << "), ";
  }
  std::cout << "\n";

  std::cout << "min_output: " << RUN.min_output << "\n";
  CreateNextTransposition(sorted_family_flows, root,
                          MakeGroups({&root}, sorted_family_flows), 0, 0);
  CheckUniqueness(RUN.out_trees);
  std::cout << "NUM_CHECKED = " << RUN.NUM_CHECKED << "\n";

  auto min_cost_trees = std::vector<TreeNodeEx>{};

  for (const auto &[cost, index] : RUN.MIN_COST_TREES.trees_) {
    min_cost_trees.emplace_back(std::move(RUN.out_trees[index]));
  }

  min_cost_trees = RUN.out_trees;

  return min_cost_trees;
}

void RememberAlgStep(const std::vector<FamilyFlow> &family_flows,
                     const TreeNodeEx &root, TreeNodeEx &node,
                     int output_index) {
  const auto num_outputs = static_cast<int>(node.outputs.size());

  if (output_index < num_outputs) {
    // auto prev_child_family_index = 0;

    // if (output_index > 0) {
    //   const auto prev_output_index = output_index - 1;

    //   if (node.child_nodes.contains(prev_output_index)) {
    //     prev_child_family_index =
    //         GetFamilyIndex(node.child_nodes.at(output_index - 1).family_id,
    //                        family_flows) +
    //         1;
    //   }
    // }

    const auto output_value = node.outputs[output_index];

    if (output_value < RUN.min_output) {
      RememberAlgStep(family_flows, root, node, output_index + 1);
      return;
    }

    if (RUN.unique_inputs.contains(output_value)) {
      RememberAlgStep(family_flows, root, node, output_index + 1);
      return;
    }

    RUN.unique_inputs.emplace(output_value);
    RUN.UNIQUE_STACK.push(output_value);

    const auto num_families = static_cast<int>(family_flows.size());

    for (auto family_i = 0; family_i < num_families + 1; ++family_i) {
      if (family_i == 0) {
        node.child_nodes.erase(output_index);
      } else {
        node.EmplaceChild(output_index, TreeNodeEx{family_flows[family_i - 1]});

        RUN.OUPUT_FAMILIES[output_value].insert(
            family_flows[family_i - 1].family_id.Get());
      }

      // for (auto output_i = output_index + 1; output_i < num_outputs;
      //      ++output_i) {
      //   node.child_nodes.erase(output_i);
      // }

      RememberAlgStep(family_flows, root, node, output_index + 1);
    }

    RUN.UNIQUE_STACK.pop();

    if (!RUN.UNIQUE_STACK.empty()) {
      RUN.OUTPUT_CHILDREN[RUN.UNIQUE_STACK.top()] +=
          RUN.OUTPUT_CHILDREN[output_value];
    }

    return;
  }

  // if (!node.child_nodes.empty()) {
  //   ++RUN.NUM_CHECKED;

  //   // auto index = 0;

  //   // for (const auto output : node.outputs) {
  //   //   if (output <= RUN.min_output) {
  //   //     if (node.child_nodes.contains(index)) {
  //   //       node.child_nodes.erase(index);
  //   //     }
  //   //   }

  //   //   ++index;
  //   // }

  //   // // if (next_level_children.empty()) {
  //   Cout() << RUN.I++ << ": " << ToString(root) << "\n";
  //   RUN.out_trees.emplace_back(root);
  //   // RUN.unique_inputs.emplace(node.input);
  //   // // }

  //   if (RUN.DEPTH > 2) {
  //     node.child_nodes.clear();
  //   }
  // }

  // if (node.parent != nullptr) {
  // }

  // if (RUN.DEPTH > 2) {
  //   return;
  // }

  for (auto &child : node.child_nodes) {
    ++RUN.DEPTH;
    RememberAlgStep(family_flows, root, child.second, 0);
    --RUN.DEPTH;
  }

  if (!node.child_nodes.empty()) {
    node.child_nodes.clear();
    return;
  }

  Cout() << RUN.I++ << ": " << RUN.DEPTH << " " << RUN.UNIQUE_STACK.top() << " "
         << ToString(root) << "\n";
  RUN.out_trees.emplace_back(root);

  // ++RUN.OUTPUT_CHILDREN[RUN.UNIQUE_STACK.top()];

  // std::cout << "UNIQUE_STACK: ";
  // auto stack_copy = RUN.UNIQUE_STACK;
  // while (!stack_copy.empty()) {
  //   std::cout << stack_copy.top() << ", ";
  //   stack_copy.pop();
  // }
  // std::cout << "\n";
}

auto RememberAlg(TreeNodeEx root, TreeNodeEx client,
                 const std::vector<FamilyFlow> &family_flows,
                 const std::vector<InputRange> &output_ranges) {
  RUN = RunData{};
  RUN.sorted_output_ranges = output_ranges;
  RUN.out_trees = std::vector<TreeNodeEx>{};
  RUN.min_output = GetMinOutput(output_ranges);
  RUN.client = std::move(client);

  RememberAlgStep(family_flows, root, root, 0);
  CheckUniqueness(RUN.out_trees);

  std::cout << "unique_inputs: ";
  for (const auto input : RUN.unique_inputs) {
    std::cout << input << ", ";
  }
  std::cout << "\n";

  // std::cout << "OUTPUT_CHILDREN: \n";
  // for (const auto &[output, num_childs] : RUN.OUTPUT_CHILDREN) {
  //   std::cout << output << ": " << num_childs << "\n";
  // }
  // std::cout << "END\n";

  std::cout << "OUTPUT_FAMILIES: \n";
  for (const auto &[output, num_childs] : RUN.OUPUT_FAMILIES) {
    std::cout << output << ": " << num_childs.size() << "\n";
  }
  std::cout << "END\n";

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

auto TreeNodeEx::CalculateCost() const -> float {
  auto cost = 0.F;

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

auto TreeNodeEx::AreOutputsLessThan(float value) const -> bool {
  return std::all_of(outputs.begin(), outputs.end(),
                     [value](const auto output) { return output < value; });
}

auto FamilyFlow::GetMinDecrementOutput() const -> float {
  auto min_output = std::numeric_limits<float>::lowest();

  for (const auto output : outputs) {
    Expects(output < 0);
    min_output = std::max(output, min_output);
  }

  return min_output;
}

auto FamilyFlow::GetMaxDecrementOutput() const -> float {
  auto max_output = std::numeric_limits<float>::max();

  for (const auto output : outputs) {
    Expects(output < 0);
    max_output = std::min(output, max_output);
  }

  return max_output;
}

auto Calculate(const CalculatorInput &input) -> std::vector<TreeNodeEx> {
  return RememberAlg(input.root, input.client, input.family_flows,
                     input.input_ranges);
}
}  // namespace vh::ponc::flow