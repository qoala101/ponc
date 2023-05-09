#include "calc_tree_node.h"

#include <algorithm>

#include "calc_resolution.h"
#include "core_i_family.h"

namespace vh::ponc::calc {
namespace {
///
auto ToCalculatorResolution(const std::vector<float> &values) {
  auto calc_values = std::vector<int>{};
  calc_values.reserve(values.size());

  std::transform(values.cbegin(), values.cend(),
                 std::back_inserter(calc_values), [](const auto value) {
                   return vh::ponc::calc::ToCalculatorResolution(value);
                 });

  return calc_values;
}
}  // namespace

///
TreeNode::TreeNode(core::FamilyId family_id) : family_id_{family_id} {}

///
TreeNode::TreeNode(core::FamilyId family_id, const std::vector<int> &outputs,
                   int node_cost)
    : family_id_{family_id},
      outputs_{outputs},
      node_cost_{node_cost},
      tree_cost_{node_cost_} {}

///
TreeNode::TreeNode(core::FamilyId family_id, const std::vector<float> &outputs,
                   float node_cost)
    : family_id_{family_id},
      outputs_{ToCalculatorResolution(outputs)},
      node_cost_{ToCalculatorResolution(node_cost)},
      tree_cost_{node_cost_} {}
}  // namespace vh::ponc::calc