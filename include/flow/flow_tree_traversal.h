#ifndef VH_PONC_FLOW_TREE_TRAVERSAL_H_
#define VH_PONC_FLOW_TREE_TRAVERSAL_H_

#include "flow_tree.h"

namespace vh::ponc::flow {
///
struct FindById {
  ///
  auto operator()(const TreeNode &tree_node) const -> bool;

  ///
  ne::NodeId node_id{};
};

///
void TraverseDepthFirst(
    const TreeNode &tree_node,
    const std::invocable<const TreeNode &> auto &visitor_before_children,
    const std::invocable<const TreeNode &> auto &visitor_after_children) {
  visitor_before_children(tree_node);

  for (const auto &child : tree_node.child_nodes) {
    TraverseDepthFirst(child.second, visitor_before_children,
                       visitor_after_children);
  }

  visitor_after_children(tree_node);
}

///
auto FindTreeNode(const TreeNode &tree_node,
                  const std::invocable<const TreeNode &> auto &predicate)
    -> std::optional<const TreeNode *> {
  if (const auto found = predicate(tree_node)) {
    return &tree_node;
  }

  for (const auto &child : tree_node.child_nodes) {
    if (const auto found_node = FindTreeNode(child.second, predicate)) {
      return found_node;
    }
  }

  return std::nullopt;
}

///
auto FindTreeNode(const FlowTree &tree,
                  const std::invocable<const TreeNode &> auto &predicate)
    -> std::optional<const TreeNode *> {
  for (const auto &root : tree.root_nodes) {
    if (const auto found_node = FindTreeNode(root, predicate)) {
      return found_node;
    }
  }

  return std::nullopt;
}
}  // namespace vh::ponc::flow

#endif  // VH_PONC_FLOW_TREE_TRAVERSAL_H_
