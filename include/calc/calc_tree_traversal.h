#ifndef VH_PONC_CALC_TREE_TRAVERSAL_H_
#define VH_PONC_CALC_TREE_TRAVERSAL_H_

#include <queue>

#include "calc_tree_node.h"

namespace vh::ponc::calc {
///
void TraverseDepthFirst(
    const TreeNode &tree_node,
    const std::invocable<const TreeNode &> auto &visitor_before_children,
    const std::invocable<const TreeNode &> auto &visitor_after_children) {
  visitor_before_children(tree_node);

  for (const auto &child : tree_node.child_nodes_) {
    TraverseDepthFirst(child.second, visitor_before_children,
                       visitor_after_children);
  }

  visitor_after_children(tree_node);
}
}  // namespace vh::ponc::calc

#endif  // VH_PONC_CALC_TREE_TRAVERSAL_H_
