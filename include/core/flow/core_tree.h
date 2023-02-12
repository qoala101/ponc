#ifndef VH_CORE_TREE_H_
#define VH_CORE_TREE_H_

#include <memory>
#include <vector>

#include "core_i_node.h"

namespace esc::core {
// ---
struct TreeNode {
  std::shared_ptr<INode> node{};
  std::map<uintptr_t, TreeNode> child_nodes{};
};

// ---
struct Tree {
  std::vector<TreeNode> root_nodes{};
};
}  // namespace esc::core

#endif  // VH_CORE_TREE_H_
