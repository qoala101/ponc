#include "flow_calculator.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <set>
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

auto CalculateOutputs(TreeNodeEx root_node) {
  TraverseDepthFirstEx(
      root_node,
      [](auto &tree_node) {
        for (auto &child : tree_node.child_nodes) {
          for (auto &output : child.second.outputs) {
            output += tree_node.outputs[child.first];
          }
        }
      },
      [](const auto &) {});

  return root_node;
}

auto InRange(float value, const InputRange &range) {
  // std::cout << "checking: " << value << " in " << range.min << " " <<
  // range.max
  //           << "\n";
  return (value >= range.min) && (value <= range.max);
}

struct Pin {
  const TreeNodeEx *tree_node{};
  int index{};

 private:
  friend auto operator==(const Pin &, const Pin &) -> bool = default;
};

auto DoesTreeHaveOutputs(TreeNodeEx tree,
                         const std::vector<InputRange> &output_ranges) {
  auto matching_pins = std::map<const InputRange *, std::vector<Pin>>{};

  TraverseDepthFirstEx(
      tree,
      [&output_ranges, &matching_pins](auto &tree_node) {
        for (auto &child : tree_node.child_nodes) {
          for (auto &output : child.second.outputs) {
            output += tree_node.outputs[child.first];
          }
        }

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

  // VH: Here must be algorithm which would find a combination of pins which
  // match all required inputs. Currently I assume that all ranges are the same
  // so its enough to have the amount of unique pins same as amount of ranges.

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

auto LastChildLessThanMin(TreeNodeEx tree, float min) {
  auto last_child_less_than_min = false;

  TraverseDepthFirstEx(
      tree,
      [min, &last_child_less_than_min](auto &tree_node) {
        if (last_child_less_than_min) {
          return;
        }

        for (auto &child : tree_node.child_nodes) {
          for (auto &output : child.second.outputs) {
            output += tree_node.outputs[child.first];
          }
        }

        if (!tree_node.child_nodes.empty()) {
          return;
        }

        if (std::all_of(tree_node.outputs.begin(), tree_node.outputs.end(),
                        [min](const auto output) { return output < min; })) {
          last_child_less_than_min = true;
        }
      },
      [](const auto &) {});

  return last_child_less_than_min;
}

void BuildAllValidTreesRecursiveStep(
    std::vector<TreeNodeEx> &out_trees, const TreeNodeEx &root,
    TreeNodeEx *last_child, float min_output,
    const std::vector<FamilyFlow> &family_flows,
    const std::vector<InputRange> &output_ranges) {
  if (DoesTreeHaveOutputs(root, output_ranges)) {
    out_trees.emplace_back(root);
    return;
  }

  if (LastChildLessThanMin(root, min_output)) {
    return;
  }

  for (const auto &family_flow : family_flows) {
    auto *new_last_child =
        &last_child->child_nodes
             .emplace(0, TreeNodeEx{.family_id = family_flow.family_id,
                                    .outputs = family_flow.outputs})
             .first->second;

    BuildAllValidTreesRecursiveStep(out_trees, root, new_last_child, min_output,
                                    family_flows, output_ranges);

    last_child->child_nodes.clear();
  }
}

auto BuildAllValidTreesRecursive(const std::vector<FamilyFlow> &family_flows,
                                 const std::vector<InputRange> &output_ranges) {
  auto min_output = std::numeric_limits<float>::max();

  for (const auto &range : output_ranges) {
    min_output = std::min(range.min, min_output);
  }

  auto out_trees = std::vector<TreeNodeEx>{};

  for (const auto &family_flow : family_flows) {
    auto root = TreeNodeEx{.family_id = family_flow.family_id,
                           .outputs = family_flow.outputs};
    auto *last_child = &root;

    BuildAllValidTreesRecursiveStep(out_trees, root, last_child, min_output,
                                    family_flows, output_ranges);
  }

  return out_trees;
}
}  // namespace

auto Calculate(const CalculatorInput &input) -> std::vector<TreeNodeEx> {
  const auto &family_flows = input.family_flows;
  const auto &input_ranges = input.input_ranges;

  // return BuildAllValidTrees(family_flows, input_ranges);
  return BuildAllValidTreesRecursive(family_flows, input_ranges);
}
}  // namespace vh::ponc::flow