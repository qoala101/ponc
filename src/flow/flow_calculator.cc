#include "flow_calculator.h"

#include <algorithm>
#include <cmath>
#include <iostream>
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

void IterateThroughOutputs(const std::vector<FamilyFlow> &family_flows,
                           std::vector<TreeNodeEx> &out_trees,
                           const TreeNodeEx &root, TreeNodeEx &node);

void IterateThroughFamilies(const std::vector<FamilyFlow> &family_flows,
                            std::vector<TreeNodeEx> &out_trees,
                            const TreeNodeEx &root, TreeNodeEx &node,
                            int output_index) {
  if (output_index >= node.GetNumOutputs()) {
    std::cout << I++ << ": ";

    ++DEPTH;
    for (auto i = 0; i < node.GetNumOutputs(); ++i) {
      if (node.child_nodes.contains(i)) {
        std::cout << node.child_nodes.at(i).family_id.Get();
        IterateThroughOutputs(family_flows, out_trees, root,
                              node.child_nodes.at(i));
      } else {
        std::cout << 0;
      }
    }
    --DEPTH;

    std::cout << " o: " << output_index << "\n";
    out_trees.emplace_back(root);

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

void IterateThroughOutputs(const std::vector<FamilyFlow> &family_flows,
                           std::vector<TreeNodeEx> &out_trees,
                           const TreeNodeEx &root, TreeNodeEx &node) {
  if (DEPTH >= 1) {
    return;
  }

  // if (out_trees.size() >= 10) {
  //   return;
  // }

  // const auto cost = root.CalculateCost();

  // if (cost >= MIN_COST) {
  //   return;
  // }

  // if (node.AreOutputsLessThan(-28)) {
  //   MIN_COST = std::min(cost, MIN_COST);
  //   return;
  // }

  IterateThroughFamilies(family_flows, out_trees, root, node, 0);
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
    IterateThroughOutputs(family_flows, out_trees, root, root);
  }

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
  return CalculateAllTrees(input.family_flows, input.input_ranges);
}
}  // namespace vh::ponc::flow