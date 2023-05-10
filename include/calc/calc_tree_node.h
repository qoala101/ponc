#ifndef VH_PONC_CALC_TREE_NODE_H_
#define VH_PONC_CALC_TREE_NODE_H_

#include <map>

#include "calc_types.h"
#include "core_i_family.h"

namespace vh::ponc::calc {
///
struct TreeNode {
  ///
  core::FamilyId family_id_{};
  ///
  Cost node_cost_{};
  ///
  Cost tree_cost_{};
  ///
  NumClients num_clients_{};
  ///
  std::vector<OutputIndex> outputs_{};
  ///
  std::map<OutputIndex, TreeNode> child_nodes_{};
};
}  // namespace vh::ponc::calc

#endif  // VH_PONC_CALC_TREE_NODE_H_
