#include "flow_calculator.h"

#include <cmath>
#include <vector>

#include "core_i_family.h"
#include "core_id_generator.h"
#include "cpp_assert.h"
#include "flow_tree.h"
#include "flow_tree_traversal.h"
#include "imgui_node_editor.h"

namespace vh::ponc::flow {
namespace {
struct FamIn {
  core::FamilyId id{};
  std::vector<float> outputs{};
  float cost{};
};

struct FamOut {
  core::FamilyId id{};
  int output_index{};
};

auto Simplex(std::vector<FamIn> fam_ins, float input, const InputRange &range) {
  auto fam_outs = std::vector<FamOut>{};

  const auto max_negative_flow_through_path = range.min - input;  // -22-6 = -28

  for (const auto &fam_in : fam_ins) {
    const auto max_fams_in_path = std::rint(
        std::floor(max_negative_flow_through_path / fam_in.outputs[0]));

    for (auto i = 0; i < max_fams_in_path; ++i) {
      fam_outs.emplace_back(FamOut{.id = fam_in.id, .output_index = 0});
    }
  }

  return fam_outs;
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

// auto IsTreeHasOutputRanges(

auto BuildAllValidTrees(std::vector<FamIn> fam_ins,
                        const std::vector<InputRange> &output_ranges) {
  auto out_trees = std::vector<TreeNodeEx>{};
  // drop_range [-28, -24]

  auto current_tree = std::optional<TreeNodeEx>{};

  for (const auto &fam_in : fam_ins) {
    if (!current_tree.has_value()) {
      current_tree = TreeNodeEx{.family_id = fam_in.id};
    }

    out_trees.emplace_back(*current_tree);
  }

  return out_trees;
}
}  // namespace

auto Calculate(const CalculatorInput &input) -> std::vector<TreeNodeEx> {
  const auto &family_flows = input.family_flows;
  const auto &input_ranges = input.input_ranges;

  auto root = TreeNodeEx{.family_id = input.family_flows.front().family_id};

  return {root};
}
}  // namespace vh::ponc::flow