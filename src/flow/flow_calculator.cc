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

auto GetNextFamilyFlow(core::FamilyId family_id,
                       const std::vector<FamilyFlow> &family_flows)
    -> std::optional<const FamilyFlow *> {
  auto family_flow = std::find_if(family_flows.begin(), family_flows.end(),
                                  [family_id](const auto &family_flow) {
                                    return family_flow.family_id == family_id;
                                  });
  Expects(family_flow != family_flows.end());
  ++family_flow;

  if (family_flow == family_flows.end()) {
    return std::nullopt;
  }

  return &*family_flow;
}

auto I = 0;
auto DEPTH = 0;
auto MIN_COST = 0.F;

auto Cout() -> std::ostream & {
  struct S : public std::ostream {};
  static auto s = S{};
  return std::cout;
  return s;
}

auto Print(const TreeNodeEx &tree) {
  auto depth = 0;

  TraverseDepthFirstExConst(
      tree,
      [&depth](const auto &node) {
        Cout() << "[" << depth << "] ";
        ++depth;
        for (auto i = 0; i < node.GetNumOutputs(); ++i) {
          if (node.child_nodes.contains(i)) {
            Cout() << node.child_nodes.at(i).family_id.Get();
          } else {
            Cout() << 0;
          }
        }

        Cout() << " ";
      },
      [&depth](const auto &) { --depth; });
  Cout() << "\n";
}

void IterateThroughFamilies(const std::vector<FamilyFlow> &family_flows,
                            std::vector<TreeNodeEx> &out_trees,
                            const TreeNodeEx &root, TreeNodeEx &node,
                            int output_index) {
  if (DEPTH >= 2) {
    return;
  }

  if (output_index >= node.GetNumOutputs()) {
    if (!node.child_nodes.empty()) {
      ++DEPTH;

      // if (DEPTH ==2){
      Cout() << I++ << ": ";
      Print(root);
      out_trees.emplace_back(root);
      // }
      // if (DEPTH >= 3) {
      //   return;
      // }

      for (auto i = 0; i < node.GetNumOutputs(); ++i) {
        if (node.child_nodes.contains(i)) {
          IterateThroughFamilies(family_flows, out_trees, root,
                                 node.child_nodes.at(i), 0);
        }
      }

      // if (!out_trees.empty() && (root == out_trees.back())) {
      //   Cout() << "VVVV\n";
      //   return;
      // }

      --DEPTH;
    }
    return;
  }

  const auto num_families = static_cast<int>(family_flows.size());
  const auto num_outputs = node.GetNumOutputs();

  auto prev_child_family_index = 0;

  if (output_index > 0) {
    const auto prev_output_index = output_index - 1;

    if (node.child_nodes.contains(prev_output_index)) {
      prev_child_family_index =
          GetFamilyIndex(node.child_nodes.at(output_index - 1).family_id,
                         family_flows) +
          1;
    }
  }

  for (auto i = prev_child_family_index; i < num_families + 1; ++i) {
    if (i == 0) {
      node.child_nodes.erase(output_index);
    } else {
      node.EmplaceChild(output_index, TreeNodeEx{family_flows[i - 1]});
    }

    for (auto i = output_index + 1; i < num_outputs; ++i) {
      node.child_nodes.erase(i);
    }

    IterateThroughFamilies(family_flows, out_trees, root, node,
                           output_index + 1);
  }
}

void CheckUniqueness(const std::vector<TreeNodeEx> &out_trees) {
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
  }
}

auto CalculateAllTrees(const std::vector<FamilyFlow> &family_flows,
                       const std::vector<InputRange> &output_ranges) {
  I = 0;
  DEPTH = 0;
  MIN_COST = std::numeric_limits<float>::max();

  auto out_trees = std::vector<TreeNodeEx>{};
  const auto min_output = GetMinOutput(output_ranges);

  for (const auto &family : family_flows) {
    auto root = TreeNodeEx{family};
    Cout() << I++ << ": ";
    Print(root);
    out_trees.emplace_back(root);
    IterateThroughFamilies(family_flows, out_trees, root, root, 0);
  }

  CheckUniqueness(out_trees);
  return out_trees;
}

void NewAlgStep(const std::vector<FamilyFlow> &family_flows,
                std::vector<TreeNodeEx> &out_trees, const TreeNodeEx &root,
                std::vector<TreeNodeEx *> &level_children);

auto GetNodeIndex(const std::vector<TreeNodeEx *> &current_level, int index)
    -> std::pair<TreeNodeEx *, int> {
  auto start_index = 0;

  for (auto *node : current_level) {
    const auto num_outputs = node->GetNumOutputs();
    const auto index_in_node = (index - start_index);

    if (index_in_node < num_outputs) {
      return std::pair{node, index_in_node};
    }

    start_index += num_outputs;
  }

  Expects(false);
}

auto Contains(const std::vector<TreeNodeEx *> &current_level, int index) {
  const auto &[node, child_index] = GetNodeIndex(current_level, index);
  return node->child_nodes.contains(child_index);
}

auto At(const std::vector<TreeNodeEx *> &current_level, int index) -> auto & {
  const auto &[node, child_index] = GetNodeIndex(current_level, index);
  return node->child_nodes.at(child_index);
}

void Erase(const std::vector<TreeNodeEx *> &current_level, int index) {
  const auto &[node, child_index] = GetNodeIndex(current_level, index);
  node->child_nodes.erase(child_index);
}

auto EmplaceChild(const std::vector<TreeNodeEx *> &current_level, int index,
                  TreeNodeEx new_node) -> auto & {
  const auto &[node, child_index] = GetNodeIndex(current_level, index);
  return node->EmplaceChild(child_index, std::move(new_node));
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

  // for (auto &group : groups) {
  //   Cout() << current_level.size() << " Made " << group.first << " size "
  //          << group.second.size() << "\n";
  // }

  return groups;
}

auto CreateNextTransposition(
    const std::vector<FamilyFlow> &family_flows,
    std::vector<TreeNodeEx> &out_trees, const TreeNodeEx &root,
    //  const std::vector<TreeNodeEx *> &current_level,
    const std::map<float, std::vector<std::pair<TreeNodeEx *, int>>>
        &current_level,
    int output_index, int group_index) -> bool {
  if (DEPTH >= 2) {
    // for (auto &group : current_level) {
    //   Cout() << group.first << " size " << group.second.size() << "\n";
    // }

    return false;
  }

  const auto num_outputs_in_group =
      group_index < current_level.size()
          ? std::next(current_level.begin(), group_index)->second.size()
          : 0;

  if (output_index >= num_outputs_in_group) {
    if (group_index >= current_level.size()) {
      ++DEPTH;
      auto next_level_nodes = std::vector<TreeNodeEx *>{};

      for (auto &group : current_level) {
        for (auto &node : group.second) {
          if (node.first->child_nodes.contains(node.second)) {
            next_level_nodes.emplace_back(
                &node.first->child_nodes.at(node.second));
          }
        }
      }

      const auto next_level = MakeGroups(next_level_nodes);

      auto result = CreateNextTransposition(family_flows, out_trees, root,
                                            next_level, 0, 0);
      --DEPTH;

      if (!result) {
        Cout() << I++ << ": ";
        Print(root);
        out_trees.emplace_back(root);
        return true;
      }

      return false;
    }

    // for (auto i = group_index + 1; i < current_level.size(); ++i) {
    //   auto &group = std::next(current_level.begin(), group_index)->second;
    //   auto index = 0;

    //   for (auto &node : group) {
    //     Erase(group, index++);
    //   }
    // }
    // out_trees.emplace_back(root);

    return CreateNextTransposition(family_flows, out_trees, root, current_level,
                                   0, group_index + 1);
  }

  const auto num_families = static_cast<int>(family_flows.size());
  auto current_group_pair = std::next(current_level.begin(), group_index);
  auto &current_group = current_group_pair->second;
  // Cout() << "current_group " << current_group_pair->first << " size "
  //        << current_group_pair->second.size() << "\n";

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

  auto result = false;

  for (auto i = prev_child_family_index; i < num_families + 1; ++i) {
    if (i == 0) {
      Erase(current_group, output_index);
    } else {
      EmplaceChild(current_group, output_index,
                   TreeNodeEx{family_flows[i - 1]});
      // out_trees.emplace_back(root);
    }

    for (auto i = output_index + 1; i < num_outputs_in_group; ++i) {
      Erase(current_group, i);
    }

    result =
        CreateNextTransposition(family_flows, out_trees, root, current_level,
                                output_index + 1, group_index) ||
        result;
  }

  return result;
}

auto NewAlg(const std::vector<FamilyFlow> &family_flows,
            const std::vector<InputRange> &output_ranges) {
  I = 0;
  DEPTH = 0;
  MIN_COST = std::numeric_limits<float>::max();

  auto out_trees = std::vector<TreeNodeEx>{};

  for (const auto &family : family_flows) {
    auto root = TreeNodeEx{family};
    CreateNextTransposition(family_flows, out_trees, root, MakeGroups({&root}),
                            0, 0);
  }

  // auto root = TreeNodeEx{family_flows[1]};
  // CreateNextTransposition(family_flows, out_trees, root, MakeGroups({&root}),
  // 0,
  //                         0);

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
  // Cout() << "EmplaceChild " << index << "\n";

  if (index == 1) {
    int a = 3;
  }

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
  // return CalculateAllTrees(input.family_flows, input.input_ranges);
  return NewAlg(input.family_flows, input.input_ranges);
}
}  // namespace vh::ponc::flow