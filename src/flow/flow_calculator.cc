#include "flow_calculator.h"

#include <algorithm>
#include <cmath>
#include <iostream>
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

auto DoesTreeHaveOutputs(const TreeNodeEx &tree,
                         const std::vector<InputRange> &output_ranges) {
  auto matching_pins = std::map<const InputRange *, std::vector<Pin>>{};

  TraverseDepthFirstExConst(
      tree,
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

auto GetMinOutput(const std::vector<InputRange> &output_ranges) {
  auto min_output = std::numeric_limits<float>::max();

  for (const auto &range : output_ranges) {
    min_output = std::min(range.min, min_output);
  }

  return min_output;
}

enum class Status { kRecordedTree, kNoSenseToAddFamiliesAtEnd, kNothing };

// NOLINTNEXTLINE(*-no-recursion)
auto BuildAllValidTreesRecursiveStep(
    std::vector<TreeNodeEx> &out_trees, const TreeNodeEx &root,
    TreeNodeEx &last_child, float min_output,
    const std::vector<FamilyFlow> &family_flows,
    const std::vector<InputRange> &output_ranges) -> Status {
  // out_trees.emplace_back(root);

  if (DoesTreeHaveOutputs(root, output_ranges)) {
    out_trees.emplace_back(root);
    return Status::kRecordedTree;
  }

  // for (const auto &family_flow : family_flows)
  const auto &family_flow = family_flows.front();
  const auto num_outputs = static_cast<int>(last_child.outputs.size());

  for (auto child_index = 0; child_index < num_outputs; ++child_index) {
    auto &new_last_child =
        last_child.EmplaceChild(child_index, TreeNodeEx{family_flow});

    if (new_last_child.AreOutputsLessThan(min_output)) {
      last_child.child_nodes.erase(child_index);
      return Status::kNoSenseToAddFamiliesAtEnd;
    }

    const auto status = BuildAllValidTreesRecursiveStep(
        out_trees, root, new_last_child, min_output, family_flows,
        output_ranges);

    if (status == Status::kNothing) {
      return Status::kNothing;
    }
  }

  return Status::kNoSenseToAddFamiliesAtEnd;
}

auto BuildAllValidTreesRecursive(const std::vector<FamilyFlow> &family_flows,
                                 const std::vector<InputRange> &output_ranges) {
  const auto min_output = GetMinOutput(output_ranges);

  auto out_trees = std::vector<TreeNodeEx>{};

  for (const auto &family_flow : family_flows) {
    auto root = TreeNodeEx{family_flow};

    if (root.AreOutputsLessThan(min_output)) {
      continue;
    }

    BuildAllValidTreesRecursiveStep(out_trees, root, root, min_output,
                                    family_flows, output_ranges);
  }

  return out_trees;
}

void AddChildren(std::vector<TreeNodeEx> &out_trees, TreeNodeEx &root,
                 int child_index, const std::vector<FamilyFlow> &family_flows,
                 const std::vector<InputRange> &output_ranges) {
  if (child_index >= static_cast<int>(root.outputs.size())) {
    return;
  }

  for (const auto &family_flow : family_flows) {
    root.EmplaceChild(child_index, TreeNodeEx{family_flow});

    if (DoesTreeHaveOutputs(root, output_ranges)) {
      out_trees.emplace_back(root);
    }

    ++child_index;
    AddChildren(out_trees, root, child_index, family_flows, output_ranges);
    --child_index;
  }
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

auto NewAlg(const std::vector<FamilyFlow> &family_flows,
            const std::vector<InputRange> &output_ranges) {
  auto out_trees = std::vector<TreeNodeEx>{};
  const auto min_output = GetMinOutput(output_ranges);

  auto root = TreeNodeEx{family_flows.front()};
  auto parent_stack = std::stack<TreeNodeEx *>{};

  auto try_next_family = [&]() {
    auto next_family_flow =
        GetNextFamilyFlow(parent_stack.top()->family_id, family_flows);

    parent_stack.pop();
    parent_stack.emplace(
        &parent_stack.top()->EmplaceChild(0, TreeNodeEx{**next_family_flow}));
    out_trees.emplace_back(root);
  };

  parent_stack.emplace(&root);
  out_trees.emplace_back(root);

  parent_stack.emplace(
      &parent_stack.top()->EmplaceChild(0, TreeNodeEx{family_flows.front()}));
  out_trees.emplace_back(root);
  parent_stack.emplace(
      &parent_stack.top()->EmplaceChild(0, TreeNodeEx{family_flows.front()}));
  out_trees.emplace_back(root);
  parent_stack.emplace(
      &parent_stack.top()->EmplaceChild(0, TreeNodeEx{family_flows.front()}));
  out_trees.emplace_back(root);
  parent_stack.emplace(
      &parent_stack.top()->EmplaceChild(0, TreeNodeEx{family_flows.front()}));
  out_trees.emplace_back(root);
  parent_stack.emplace(
      &parent_stack.top()->EmplaceChild(0, TreeNodeEx{family_flows.front()}));
  out_trees.emplace_back(root);

  parent_stack.emplace(
      &parent_stack.top()->EmplaceChild(0, TreeNodeEx{family_flows.front()}));
  out_trees.emplace_back(root);

  try_next_family();
  try_next_family();
  try_next_family();

  parent_stack.pop();
  parent_stack.top()->child_nodes.erase(0);
  out_trees.emplace_back(root);

  parent_stack.pop();
  parent_stack.emplace(
      &parent_stack.top()->EmplaceChild(1, TreeNodeEx{family_flows.front()}));
  out_trees.emplace_back(root);

  parent_stack.emplace(
      &parent_stack.top()->EmplaceChild(0, TreeNodeEx{family_flows.front()}));
  out_trees.emplace_back(root);

  try_next_family();
  try_next_family();
  try_next_family();

  // after this

  parent_stack.pop();
  parent_stack.top()->child_nodes.erase(0);
  out_trees.emplace_back(root);

  parent_stack.pop();

  if (parent_stack.top()->child_nodes.size() >=
      parent_stack.top()->outputs.size()) {
    auto next_family_flow = GetNextFamilyFlow(
        parent_stack.top()->child_nodes.at(0).family_id, family_flows);

    parent_stack.top()->child_nodes.clear();
    out_trees.emplace_back(root);

    if (next_family_flow.has_value()) {
      parent_stack.emplace(
          &parent_stack.top()->EmplaceChild(0, TreeNodeEx{**next_family_flow}));
      out_trees.emplace_back(root);
    }
  }

  try_next_family();
  try_next_family();

  parent_stack.pop();
  parent_stack.top()->child_nodes.erase(0);
  out_trees.emplace_back(root);

  parent_stack.pop();

  auto next_family_flow = GetNextFamilyFlow(
      parent_stack.top()->child_nodes.at(0).family_id, family_flows);

  parent_stack.top()->child_nodes.erase(0);
  out_trees.emplace_back(root);

  parent_stack.emplace(
      &parent_stack.top()->EmplaceChild(0, TreeNodeEx{**next_family_flow}));
  out_trees.emplace_back(root);

  parent_stack.emplace(
      &parent_stack.top()->EmplaceChild(0, TreeNodeEx{family_flows.front()}));
  out_trees.emplace_back(root);

  try_next_family();
  try_next_family();
  try_next_family();

  parent_stack.pop();
  parent_stack.top()->child_nodes.erase(0);
  out_trees.emplace_back(root);

  parent_stack.pop();

  parent_stack.emplace(
      &parent_stack.top()->EmplaceChild(1, TreeNodeEx{family_flows.front()}));
  out_trees.emplace_back(root);

  parent_stack.emplace(
      &parent_stack.top()->EmplaceChild(0, TreeNodeEx{family_flows.front()}));
  out_trees.emplace_back(root);

  parent_stack.emplace(
      &parent_stack.top()->EmplaceChild(0, TreeNodeEx{family_flows.front()}));
  out_trees.emplace_back(root);

  try_next_family();
  try_next_family();
  try_next_family();

  parent_stack.pop();
  parent_stack.top()->child_nodes.erase(0);
  out_trees.emplace_back(root);

  parent_stack.pop();

  parent_stack.emplace(
      &parent_stack.top()->EmplaceChild(1, TreeNodeEx{family_flows.front()}));
  out_trees.emplace_back(root);

  parent_stack.emplace(
      &parent_stack.top()->EmplaceChild(0, TreeNodeEx{family_flows.front()}));
  out_trees.emplace_back(root);

  try_next_family();
  try_next_family();
  try_next_family();

  parent_stack.pop();
  parent_stack.top()->child_nodes.erase(0);
  out_trees.emplace_back(root);

  parent_stack.pop();

  next_family_flow = GetNextFamilyFlow(
      parent_stack.top()->child_nodes.at(0).family_id, family_flows);

  parent_stack.top()->child_nodes.clear();
  out_trees.emplace_back(root);

  parent_stack.emplace(
      &parent_stack.top()->EmplaceChild(0, TreeNodeEx{**next_family_flow}));
  out_trees.emplace_back(root);

  try_next_family();
  try_next_family();

  parent_stack.pop();
  parent_stack.top()->child_nodes.erase(0);
  out_trees.emplace_back(root);

  parent_stack.pop();

  next_family_flow = GetNextFamilyFlow(
      parent_stack.top()->child_nodes.at(1).family_id, family_flows);

  parent_stack.emplace(
      &parent_stack.top()->EmplaceChild(1, TreeNodeEx{**next_family_flow}));
  out_trees.emplace_back(root);

  parent_stack.emplace(
      &parent_stack.top()->EmplaceChild(0, TreeNodeEx{family_flows.front()}));
  out_trees.emplace_back(root);

  try_next_family();
  try_next_family();
  try_next_family();

  parent_stack.pop();
  parent_stack.top()->child_nodes.erase(0);
  out_trees.emplace_back(root);

  parent_stack.pop();

  next_family_flow = GetNextFamilyFlow(
      parent_stack.top()->child_nodes.at(0).family_id, family_flows);

  parent_stack.top()->child_nodes.clear();
  out_trees.emplace_back(root);

  parent_stack.emplace(
      &parent_stack.top()->EmplaceChild(0, TreeNodeEx{**next_family_flow}));
  out_trees.emplace_back(root);

  return out_trees;

  // parent_stack.pop();
  // parent_stack.top()->child_nodes.erase(0);
  // out_trees.emplace_back(root);

  // parent_stack.pop();
  // parent_stack.top()->child_nodes.clear();
  // out_trees.emplace_back(root);

  // auto next_family_flow =
  //     GetNextFamilyFlow(parent_stack.top()->family_id, family_flows);

  // parent_stack.pop();
  // parent_stack.emplace(
  //     &parent_stack.top()->EmplaceChild(0, TreeNodeEx{**next_family_flow}));
  // out_trees.emplace_back(root);

  // parent_stack.emplace(
  //     &parent_stack.top()->EmplaceChild(0,
  //     TreeNodeEx{family_flows.front()}));
  // out_trees.emplace_back(root);

  // try_next_family();
  // try_next_family();
  // try_next_family();

  // parent_stack.pop();
  // parent_stack.top()->child_nodes.erase(0);
  // out_trees.emplace_back(root);

  // parent_stack.pop();
  // parent_stack.emplace(
  //     &parent_stack.top()->EmplaceChild(1,
  //     TreeNodeEx{family_flows.front()}));
  // out_trees.emplace_back(root);

  // parent_stack.emplace(
  //     &parent_stack.top()->EmplaceChild(0,
  //     TreeNodeEx{family_flows.front()}));
  // out_trees.emplace_back(root);

  // parent_stack.emplace(
  //     &parent_stack.top()->EmplaceChild(0,
  //     TreeNodeEx{family_flows.front()}));
  // out_trees.emplace_back(root);

  // try_next_family();
  // try_next_family();
  // try_next_family();

  // parent_stack.pop();
  // parent_stack.top()->child_nodes.erase(0);
  // out_trees.emplace_back(root);

  // parent_stack.pop();
  // parent_stack.emplace(
  //     &parent_stack.top()->EmplaceChild(1,
  //     TreeNodeEx{family_flows.front()}));
  // out_trees.emplace_back(root);

  // parent_stack.emplace(
  //     &parent_stack.top()->EmplaceChild(0,
  //     TreeNodeEx{family_flows.front()}));
  // out_trees.emplace_back(root);

  // try_next_family();
  // try_next_family();
  // try_next_family();

  // parent_stack.pop();
  // parent_stack.top()->child_nodes.erase(0);
  // out_trees.emplace_back(root);

  // parent_stack.pop();

  // next_family_flow = GetNextFamilyFlow(
  //     parent_stack.top()->child_nodes.at(0).family_id, family_flows);

  // parent_stack.top()->child_nodes.clear();
  // out_trees.emplace_back(root);

  // parent_stack.emplace(
  //     &parent_stack.top()->EmplaceChild(0, TreeNodeEx{**next_family_flow}));
  // out_trees.emplace_back(root);

  // try_next_family();
  // try_next_family();

  // parent_stack.pop();
  // parent_stack.top()->child_nodes.erase(0);
  // out_trees.emplace_back(root);

  // parent_stack.pop();

  // next_family_flow = GetNextFamilyFlow(
  //     parent_stack.top()->child_nodes.at(1).family_id, family_flows);

  // parent_stack.emplace(
  //     &parent_stack.top()->EmplaceChild(1, TreeNodeEx{**next_family_flow}));
  // out_trees.emplace_back(root);

  // parent_stack.emplace(
  //     &parent_stack.top()->EmplaceChild(0,
  //     TreeNodeEx{family_flows.front()}));
  // out_trees.emplace_back(root);

  // try_next_family();
  // try_next_family();
  // try_next_family();

  // parent_stack.pop();
  // parent_stack.top()->child_nodes.erase(0);
  // out_trees.emplace_back(root);

  // parent_stack.pop();

  // next_family_flow = GetNextFamilyFlow(
  //     parent_stack.top()->child_nodes.at(0).family_id, family_flows);

  // parent_stack.top()->child_nodes.clear();
  // out_trees.emplace_back(root);

  // parent_stack.emplace(
  //     &parent_stack.top()->EmplaceChild(0, TreeNodeEx{**next_family_flow}));
  // out_trees.emplace_back(root);

  // parent_stack.emplace(
  //     &parent_stack.top()->EmplaceChild(0,
  //     TreeNodeEx{family_flows.front()}));
  // out_trees.emplace_back(root);

  // try_next_family();
  // try_next_family();
  // try_next_family();

  // parent_stack.pop();
  // parent_stack.top()->child_nodes.erase(0);
  // out_trees.emplace_back(root);

  // parent_stack.pop();
  // parent_stack.emplace(
  //     &parent_stack.top()->EmplaceChild(1,
  //     TreeNodeEx{family_flows.front()}));
  // out_trees.emplace_back(root);

  // parent_stack.emplace(
  //     &parent_stack.top()->EmplaceChild(0,
  //     TreeNodeEx{family_flows.front()}));
  // out_trees.emplace_back(root);

  // parent_stack.emplace(
  //     &parent_stack.top()->EmplaceChild(0,
  //     TreeNodeEx{family_flows.front()}));
  // out_trees.emplace_back(root);

  // try_next_family();
  // try_next_family();
  // try_next_family();

  // parent_stack.pop();
  // parent_stack.top()->child_nodes.erase(0);
  // out_trees.emplace_back(root);

  // parent_stack.pop();
  // parent_stack.emplace(
  //     &parent_stack.top()->EmplaceChild(1,
  //     TreeNodeEx{family_flows.front()}));
  // out_trees.emplace_back(root);

  // parent_stack.emplace(
  //     &parent_stack.top()->EmplaceChild(0,
  //     TreeNodeEx{family_flows.front()}));
  // out_trees.emplace_back(root);

  // try_next_family();
  // try_next_family();
  // try_next_family();

  // return out_trees;
}

auto NewAlgRecursive(const std::vector<FamilyFlow> &family_flows,
                     const std::vector<InputRange> &output_ranges) {
  auto out_trees = std::vector<TreeNodeEx>{};
  const auto min_output = GetMinOutput(output_ranges);

  auto root = TreeNodeEx{family_flows.front()};
  auto parent_stack = std::stack<TreeNodeEx *>{};

  parent_stack.emplace(&root);
  out_trees.emplace_back(root);

  auto STEP = 1;
  auto MAX_STEPS = 45;

  auto *next_family = &family_flows.front();
  auto next_child_index = 0;
  auto skip_adding_child = false;

  auto COUNT = 0;

  const auto get_next_family = [&](const auto &tree_node) {
    return GetNextFamilyFlow(tree_node.family_id, family_flows);
  };

  const auto can_add_new_child = [&]() {
    return !parent_stack.top()->AreOutputsLessThan(min_output);
  };

  const auto add_new_child = [&]() {
    parent_stack.emplace(&parent_stack.top()->EmplaceChild(
        next_child_index, TreeNodeEx{*next_family}));
    out_trees.emplace_back(root);
  };

  while (STEP++ < MAX_STEPS) {
    if (can_add_new_child()) {
      add_new_child();

      // next_family = &family_flows.front();
      // next_child_index = 0;
      continue;
    }

    break;

    const auto *child = parent_stack.top();

    parent_stack.pop();

    const auto child_index = parent_stack.top()->GetChildIndex(child);

    if (const auto next_family =
            get_next_family(parent_stack.top()->child_nodes.at(child_index))) {
      parent_stack.emplace(&parent_stack.top()->EmplaceChild(
          child_index, TreeNodeEx{**next_family}));
      out_trees.emplace_back(root);
      continue;
    }

    if (child_index > 0) {
      ++COUNT;
      std::cout << "(child_index >0)\n";
    }

    parent_stack.top()->child_nodes.erase(child_index);
    out_trees.emplace_back(root);

    // insert logic here
    if (child_index > 0) {
      const auto prev_child_index = child_index - 1;
      auto &prev_child = parent_stack.top()->child_nodes.at(prev_child_index);

      if (const auto prev_next_family = get_next_family(prev_child)) {
        next_family = *prev_next_family;
      } else {
        Expects(false);
      }

      // skip_adding_child = true;

      // parent_stack.pop();
      // parent_stack.emplace(&prev_child);
      continue;
    }

    parent_stack.pop();

    if (parent_stack.top()->GetNumChildren() <
        parent_stack.top()->GetNumOutputs()) {
      next_child_index = parent_stack.top()->GetNumChildren();
      continue;
    }

    const auto last_child_index = parent_stack.top()->GetNumChildren() - 1;

    if (const auto last_child_next_family = get_next_family(
            parent_stack.top()->child_nodes.at(last_child_index))) {
      parent_stack.emplace(&parent_stack.top()->EmplaceChild(
          last_child_index, TreeNodeEx{**last_child_next_family}));
      out_trees.emplace_back(root);
      continue;
    }

    break;
  }

  return out_trees;
}

auto LoopAlgStep(const std::vector<FamilyFlow> &family_flows,
                 const std::vector<InputRange> &output_ranges,
                 std::vector<TreeNodeEx> &out_trees, const TreeNodeEx &root,
                 TreeNodeEx &node, float min_output, int depth) {
  // if (node.AreOutputsLessThan(min_output)) {

  // if (out_trees.size() > 100) {
  //   return;
  // }

  if (depth >= 3) {
    out_trees.emplace_back(root);
    return;
  }

  const auto num_outputs = static_cast<int>(node.outputs.size());
  const auto num_families = static_cast<int>(family_flows.size());

  for (auto outer_o = 0; outer_o < num_outputs - 1; ++outer_o) {
    for (auto outer_f = 0; outer_f < num_families + 1; ++outer_f) {
      auto *outer_child = static_cast<TreeNodeEx *>(nullptr);

      if (outer_f < num_families) {
        outer_child =
            &node.EmplaceChild(outer_o, TreeNodeEx{family_flows[outer_f]});

        ++depth;
        LoopAlgStep(family_flows, output_ranges, out_trees, root, *outer_child,
                    min_output, depth);
        --depth;
      } else {
        outer_child = nullptr;
        node.child_nodes.erase(outer_o);
      }

      for (auto inner_o = outer_o + 1; inner_o < num_outputs; ++inner_o) {
        for (auto inner_f = 0; inner_f < num_families + 1; ++inner_f) {
          if (inner_f < num_families) {
            auto &child =
                node.EmplaceChild(inner_o, TreeNodeEx{family_flows[inner_f]});

            ++depth;
            LoopAlgStep(family_flows, output_ranges, out_trees, root, child,
                        min_output, depth);
            --depth;
          } else {
            node.child_nodes.erase(inner_o);
          }
        }
      }
    }

    node.child_nodes.erase(outer_o);
    // out_trees.emplace_back(root);
  }
}

auto I = 0;

void PrintStep(std::vector<int> &outputs, const std::vector<int> &families,
               int output_index) {
  if (output_index >= outputs.size()) {
    std::cout << I++ << ": ";

    for (const auto f : outputs) {
      std::cout << f;
    }

    std::cout << " o: " << output_index << "\n";

    return;
  }

  for (const auto f : families) {
    outputs[output_index] = f;

    for (auto i = output_index + 1; i < outputs.size(); ++i) {
      outputs[i] = families.front();
    }

    PrintStep(outputs, families, output_index + 1);
  }
}

auto Print(const std::vector<FamilyFlow> &family_flows, int num_outputs) {
  auto output_index = 0;

  std::cout << "fam size: " << family_flows.size() << "\n";
  std::cout << "outs size: " << num_outputs << "\n";

  auto families = std::vector{0, 2, 4};
  auto ouput_families = std::vector{0, 0, 0, 0};

  I = 0;
  PrintStep(ouput_families, families, output_index);
}

void PrintWithCorrectArgsStep(TreeNodeEx &node,
                              const std::vector<FamilyFlow> &family_flows,
                              int output_index) {
  if (output_index >= node.GetNumOutputs()) {
    std::cout << I++ << ": ";

    for (auto i = 0; i < node.GetNumOutputs(); ++i) {
      if (node.child_nodes.contains(i)) {
        std::cout << node.child_nodes.at(i).family_id.Get();
      } else {
        std::cout << 0;
      }
    }

    std::cout << " o: " << output_index << "\n";

    return;
  }

  for (auto i = 0; i < family_flows.size() + 1; ++i) {
    if (i == 0) {
      node.child_nodes.erase(output_index);
    } else {
      node.EmplaceChild(output_index, TreeNodeEx{family_flows[i - 1]});
    }

    for (auto i = output_index + 1; i < node.GetNumOutputs(); ++i) {
      node.child_nodes.erase(i);
    }

    PrintWithCorrectArgsStep(node, family_flows, output_index + 1);
  }
}

auto PrintWithCorrectArgs(const std::vector<FamilyFlow> &family_flows,
                          int num_outputs) {
  auto output_index = 0;

  auto node = TreeNodeEx{family_flows[1]};

  I = 0;
  PrintWithCorrectArgsStep(node, family_flows, output_index);
}

void LoopAlgStep_v2(const std::vector<FamilyFlow> &family_flows,
                    std::vector<TreeNodeEx> &out_trees, const TreeNodeEx &root,
                    TreeNodeEx &node, int depth);

void LoopAlgInnerStep_v2(const std::vector<FamilyFlow> &family_flows,
                         std::vector<TreeNodeEx> &out_trees,
                         const TreeNodeEx &root, TreeNodeEx &node, int depth,
                         int output_index) {
  if (output_index >= node.GetNumOutputs()) {
    // std::cout << I++ << ": ";

    ++depth;
    for (auto i = 0; i < node.GetNumOutputs(); ++i) {
      if (node.child_nodes.contains(i)) {
        // std::cout << node.child_nodes.at(i).family_id.Get();
        LoopAlgStep_v2(family_flows, out_trees, root, node.child_nodes.at(i),
                       depth);
      } else {
        // std::cout << 0;
      }
    }
    --depth;

    // std::cout << " o: " << output_index << "\n";
    out_trees.emplace_back(root);

    return;
  }

  for (auto i = 0; i < family_flows.size() + 1; ++i) {
    if (i == 0) {
      node.child_nodes.erase(output_index);
    } else {
      node.EmplaceChild(output_index, TreeNodeEx{family_flows[i - 1]});
    }

    for (auto i = output_index + 1; i < node.GetNumOutputs(); ++i) {
      node.child_nodes.erase(i);
    }

    LoopAlgInnerStep_v2(family_flows, out_trees, root, node, depth,
                        output_index + 1);
  }
}

void LoopAlgStep_v2(const std::vector<FamilyFlow> &family_flows,
                    std::vector<TreeNodeEx> &out_trees, const TreeNodeEx &root,
                    TreeNodeEx &node, int depth) {
  if (depth >= 2) {
    // out_trees.emplace_back(root);
    // std::cout << "+";
    return;
  }

  auto output_index = 0;
  LoopAlgInnerStep_v2(family_flows, out_trees, root, node, depth, output_index);

  // auto iterating_index
}

auto LoopAlg(const std::vector<FamilyFlow> &family_flows,
             const std::vector<InputRange> &output_ranges) {
  auto out_trees = std::vector<TreeNodeEx>{};
  const auto min_output = GetMinOutput(output_ranges);

  for (const auto &family : family_flows) {
    auto root = TreeNodeEx{family};
    // out_trees.emplace_back(root);
    auto depth = 0;
    LoopAlgStep_v2(family_flows, out_trees, root, root, depth);
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
  const auto &family_flows = input.family_flows;
  const auto &input_ranges = input.input_ranges;

  // return BuildAllValidTrees(family_flows, input_ranges);
  // return BuildAllValidTreesRecursive(family_flows,
  // input_ranges); return NewAlg(family_flows, input_ranges);
  // return NewAlgRecursive(family_flows, input_ranges);
  return LoopAlg(family_flows, input_ranges);
  // PrintWithCorrectArgs(family_flows, 4);
  // return {};
}
}  // namespace vh::ponc::flow