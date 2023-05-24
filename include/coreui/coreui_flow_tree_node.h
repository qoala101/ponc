/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_COREUI_FLOW_TREE_H_
#define VH_PONC_COREUI_FLOW_TREE_H_

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "core_i_family.h"
#include "core_id_value.h"
#include "cpp_safe_ptr.h"

namespace vh::ponc::coreui {
///
class Node;

///
struct TreeNode {
  ///
  cpp::SafePtr<Node> node;
  ///
  std::vector<TreeNode> child_nodes{};
  ///
  std::unordered_map<core::IdValue<core::FamilyId>, int>
      num_children_per_family{};
};
}  // namespace vh::ponc::coreui

#endif  // VH_PONC_COREUI_FLOW_TREE_H_
