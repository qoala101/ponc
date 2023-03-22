#ifndef VH_PONC_COREUI_FLOW_TREE_H_
#define VH_PONC_COREUI_FLOW_TREE_H_

#include <cstdint>
#include <unordered_map>
#include <vector>

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
  std::unordered_map<uintptr_t, int> num_children_per_family{};
};

///
struct FlowTree {
  ///
  std::vector<TreeNode> root_nodes{};
};
}  // namespace vh::ponc::coreui

#endif  // VH_PONC_COREUI_FLOW_TREE_H_
