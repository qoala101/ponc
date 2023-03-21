#ifndef VH_COREUI_FLOW_TREE_H_
#define VH_COREUI_FLOW_TREE_H_

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "cpp_safe_ptr.h"

namespace esc::coreui {
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
}  // namespace esc::coreui

#endif  // VH_COREUI_FLOW_TREE_H_
