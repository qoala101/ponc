#ifndef VH_CORE_TREE_H_
#define VH_CORE_TREE_H_

#include <functional>
#include <memory>
#include <vector>

#include "core_i_node.h"

namespace esc::flow {
struct TreeNode;

using Visitor = std::function<void(const TreeNode &)>;

// ---
struct TreeNode {
  std::shared_ptr<core::INode> node{};
  std::map<uintptr_t, TreeNode> child_nodes{};

 private:
  friend void TraverseDepthFirst(
      const TreeNode &tree_node,
      const std::optional<Visitor> &visit_before_children,
      const std::optional<Visitor> &visit_after_children);
};

// ---
struct Tree {
  std::vector<TreeNode> root_nodes{};
};
}  // namespace esc::flow

#endif  // VH_CORE_TREE_H_
