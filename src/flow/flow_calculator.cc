#include "flow_calculator.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <istream>
#include <iterator>
#include <limits>
#include <set>
#include <stack>
#include <unordered_set>
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

struct Pin {
  const TreeNodeEx *tree_node{};
  int index{};

 private:
  friend auto operator==(const Pin &, const Pin &) -> bool = default;
};

auto GetMinOutput(const std::vector<InputRange> &output_ranges) {
  auto min_output = std::numeric_limits<float>::max();

  for (const auto &range : output_ranges) {
    min_output = std::min(range.min, min_output);
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

auto I = 0;
auto DEPTH = 0;
auto MIN_COST = 0.F;

auto Cout() -> std::ostream & {
  struct S : public std::ostream {};
  static auto s = S{};
  // return std::cout;
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

auto MakeGroups(const std::vector<TreeNodeEx *> &current_level) {
  auto groups = std::map<float, std::vector<std::pair<TreeNodeEx *, int>>>{};

  for (auto *node : current_level) {
    auto index = 0;

    for (const auto output : node->outputs) {
      groups[output].emplace_back(node, index++);
    }
  }

  return groups;
}

auto GetNextLevelNodes(
    const std::map<float, std::vector<std::pair<TreeNodeEx *, int>>>
        &current_level) {
  auto next_level_nodes = std::vector<TreeNodeEx *>{};

  for (const auto &group : current_level) {
    for (const auto &[node, index] : group.second) {
      if (node->child_nodes.contains(index)) {
        next_level_nodes.emplace_back(&node->child_nodes.at(index));
      }
    }
  }

  return next_level_nodes;
}

void CreateNextTransposition(
    const std::vector<FamilyFlow> &family_flows,
    std::vector<TreeNodeEx> &out_trees, const TreeNodeEx &root,
    //  const std::vector<TreeNodeEx *> &current_level,
    const std::map<float, std::vector<std::pair<TreeNodeEx *, int>>>
        &current_level,
    int output_index, int group_index) {
  if (DEPTH >= 3) {
    return;
  }

  const auto num_groups = static_cast<int>(current_level.size());
  const auto num_outputs_in_group =
      group_index < num_groups
          ? static_cast<int>(
                std::next(current_level.begin(), group_index)->second.size())
          : 0;

  if (output_index >= num_outputs_in_group) {
    if (group_index >= num_groups) {
      auto empty_transposition = true;

      for (const auto &group : current_level) {
        for (const auto &[node, index] : group.second) {
          if (node->child_nodes.contains(index)) {
            empty_transposition = false;
            break;
          }
        }

        if (!empty_transposition) {
          break;
        }
      }

      const auto next_level_nodes = GetNextLevelNodes(current_level);

      if (!empty_transposition) {
        for (auto *node : next_level_nodes) {
          node->child_nodes.clear();
        }

        Cout() << I++ << ": " << ToString(root) << "\n";
        out_trees.emplace_back(root);
      }

      ++DEPTH;
      const auto next_level = MakeGroups(next_level_nodes);

      CreateNextTransposition(family_flows, out_trees, root, next_level, 0, 0);
      --DEPTH;
      return;
    }

    return CreateNextTransposition(family_flows, out_trees, root, current_level,
                                   0, group_index + 1);
  }

  const auto num_families = static_cast<int>(family_flows.size());
  auto current_group_pair = std::next(current_level.begin(), group_index);
  const auto &current_group = current_group_pair->second;

  auto prev_child_family_index = 0;

  if (output_index > 0) {
    const auto prev_output_index = output_index - 1;

    if (Contains(current_group, prev_output_index)) {
      prev_child_family_index =
          GetFamilyIndex(At(current_group, output_index - 1).family_id,
                         family_flows) +
          1;
    }
  }

  for (auto family_i = prev_child_family_index; family_i < num_families + 1;
       ++family_i) {
    if (family_i == 0) {
      Erase(current_group, output_index);
    } else {
      EmplaceChild(current_group, output_index,
                   TreeNodeEx{family_flows[family_i - 1]});
    }

    for (auto output_i = output_index + 1; output_i < num_outputs_in_group;
         ++output_i) {
      Erase(current_group, output_i);
    }

    CreateNextTransposition(family_flows, out_trees, root, current_level,
                            output_index + 1, group_index);
  }
}

auto NewAlg(TreeNodeEx root, const std::vector<FamilyFlow> &family_flows,
            const std::vector<InputRange> &output_ranges) {
  I = 0;
  DEPTH = 0;
  MIN_COST = std::numeric_limits<float>::max();

  auto out_trees = std::vector<TreeNodeEx>{};

  CreateNextTransposition(family_flows, out_trees, root, MakeGroups({&root}), 0,
                          0);
  CheckUniqueness(out_trees);
  return out_trees;
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

auto TreeNodeEx::HasOutputs(const std::vector<InputRange> &output_ranges) const
    -> bool {
  auto matching_pins = std::map<const InputRange *, std::vector<Pin>>{};

  TraverseDepthFirstExConst(
      *this,
      [&output_ranges, &matching_pins](auto &tree_node) {
        auto index = 0;

        for (const auto output : tree_node.outputs) {
          const auto index_scope = cpp::Scope{[&index]() { ++index; }};

          if (tree_node.child_nodes.contains(index)) {
            continue;
          }

          for (const auto &range : output_ranges) {
            if (InRange(output, range)) {
              matching_pins[&range].emplace_back(
                  Pin{.tree_node = &tree_node, .index = index});
            }
          }
        }
      },
      [](const auto &) {});

  if (matching_pins.size() < output_ranges.size()) {
    return false;
  }

  // VH: Here must be algorithm which would find a combination
  // of pins which match all required inputs. Currently I assume
  // that all ranges are the same so its enough to have the
  // amount of unique pins same as amount of ranges.

  auto unique_pins = std::vector<Pin>{};

  for (const auto &[range, pins_in_range] : matching_pins) {
    for (const auto &pin : pins_in_range) {
      if (std::none_of(
              unique_pins.begin(), unique_pins.end(),
              [&pin](const auto &taken_pin) { return taken_pin == pin; })) {
        unique_pins.emplace_back(pin);
      }
    }
  }

  return unique_pins.size() >= output_ranges.size();
}

auto TreeNodeEx::EmplaceChild(int index, TreeNodeEx child) -> TreeNodeEx & {
  for (auto &output : child.outputs) {
    output += outputs[index];
  }

  return child_nodes[index] = std::move(child);
}

auto TreeNodeEx::AreOutputsLessThan(float value) const -> bool {
  return std::all_of(outputs.begin(), outputs.end(),
                     [value](const auto output) { return output < value; });
}

auto Calculate(const CalculatorInput &input) -> std::vector<TreeNodeEx> {
  return NewAlg(input.root, input.family_flows, input.input_ranges);
}
}  // namespace vh::ponc::flow