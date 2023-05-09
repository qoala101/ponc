#ifndef VH_PONC_CALC_TREE_NODE_H_
#define VH_PONC_CALC_TREE_NODE_H_

#include <map>

#include "core_i_family.h"

namespace vh::ponc::calc {
///
using Cost = int;
///
using FlowValue = int;
///
using NumClients = int;
///
using OutputIndex = int;

///
struct TreeNode {
  ///
  TreeNode() = default;
  ///
  explicit TreeNode(core::FamilyId family_id);
  ///
  TreeNode(core::FamilyId family_id, const std::vector<int> &outputs,
           int node_cost = {});
  ///
  TreeNode(core::FamilyId family_id, const std::vector<float> &outputs,
           float node_cost = {});

  ///
  core::FamilyId family_id_{};
  ///
  std::vector<OutputIndex> outputs_{};
  ///
  Cost node_cost_{};
  ///
  NumClients num_clients_{};
  ///
  Cost tree_cost_{};
  ///
  FlowValue input_{};
  ///
  std::map<OutputIndex, TreeNode> child_nodes_{};
};
}  // namespace vh::ponc::calc

#endif  // VH_PONC_CALC_TREE_NODE_H_
