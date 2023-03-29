#include "flow_calculator.h"

#include <cmath>
#include <iostream>
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
  std::cout << "checking: " << value << " in " << range.min << " " << range.max
            << "\n";
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

auto BuildAllValidTrees(const std::vector<FamilyFlow> &family_flows,
                        const std::vector<InputRange> &output_ranges) {
  auto out_trees = std::vector<TreeNodeEx>{};
  auto fake_root = TreeNodeEx{};

  for (const auto &family_flow_0 : family_flows) {
    auto *root_0 =
        &fake_root.child_nodes
             .emplace(0, TreeNodeEx{.family_id = family_flow_0.family_id,
                                    .outputs = family_flow_0.outputs})
             .first->second;

    if (DoesTreeHaveOutputs(*root_0, output_ranges)) {
      out_trees.emplace_back(*root_0);
    } else {
      for (const auto &family_flow_1 : family_flows) {
        auto *root_1 =
            &root_0->child_nodes
                 .emplace(0, TreeNodeEx{.family_id = family_flow_1.family_id,
                                        .outputs = family_flow_1.outputs})
                 .first->second;

        if (DoesTreeHaveOutputs(*root_0, output_ranges)) {
          out_trees.emplace_back(*root_0);
        } else {
          for (const auto &family_flow_2 : family_flows) {
            auto *root_2 =
                &root_1->child_nodes
                     .emplace(0,
                              TreeNodeEx{.family_id = family_flow_2.family_id,
                                         .outputs = family_flow_2.outputs})
                     .first->second;

            if (DoesTreeHaveOutputs(*root_0, output_ranges)) {
              out_trees.emplace_back(*root_0);
            } else {
              for (const auto &family_flow_3 : family_flows) {
                auto *root_3 =
                    &root_2->child_nodes
                         .emplace(
                             0, TreeNodeEx{.family_id = family_flow_3.family_id,
                                           .outputs = family_flow_3.outputs})
                         .first->second;

                if (DoesTreeHaveOutputs(*root_0, output_ranges)) {
                  out_trees.emplace_back(*root_0);
                }

                root_2->child_nodes.clear();
              }
            }

            root_1->child_nodes.clear();
          }
        }

        root_0->child_nodes.clear();
      }
    }

    fake_root.child_nodes.clear();
  }

  return out_trees;
}
}  // namespace

auto Calculate(const CalculatorInput &input) -> std::vector<TreeNodeEx> {
  const auto &family_flows = input.family_flows;
  const auto &input_ranges = input.input_ranges;

  return BuildAllValidTrees(family_flows, input_ranges);
}
}  // namespace vh::ponc::flow