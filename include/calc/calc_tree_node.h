/**
 * PON Calculator https://github.com/qoala101/ponc
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_CALC_TREE_NODE_H_
#define VH_PONC_CALC_TREE_NODE_H_

#include <map>

#include "calc_types.h"
#include "core_i_family.h"

namespace vh::ponc::calc {
///
struct TreeNode {
  ///
  core::FamilyId family_id{};
  ///
  Cost node_cost{};
  ///
  Cost tree_cost{};
  ///
  NumClients num_clients{};
  ///
  std::vector<OutputIndex> outputs{};
  ///
  std::map<OutputIndex, TreeNode> child_nodes{};
};
}  // namespace vh::ponc::calc

#endif  // VH_PONC_CALC_TREE_NODE_H_
