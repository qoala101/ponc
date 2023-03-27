#ifndef VH_PONC_FLOW_TREE_TRAVERSAL_H_
#define VH_PONC_FLOW_TREE_TRAVERSAL_H_

#include <queue>

#include "flow_tree.h"
#include "imgui_node_editor.h"

namespace vh::ponc::flow {
namespace detail {
///
void TraverseBreadthFirstImpl(
    std::queue<const TreeNode *> &queue,
    const std::invocable<const TreeNode &> auto &visitor) {
  while (!queue.empty()) {
    for (const auto &child : queue.front()->child_nodes) {
      visitor(child);
      queue.emplace(&child.second);
    }
  }
}
}  // namespace detail

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
void TraverseBreadthFirst(
    const TreeNode &tree_node,
    const std::invocable<const TreeNode &> auto &visitor) {
  auto queue = std::queue<const TreeNode *>{};
  queue.emplace(&tree_node);
  TraverseBreadthFirstImpl(queue, visitor);
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
auto FindTreeNode(const TreeNode &tree_node, ne::NodeId node_id)
    -> const TreeNode &;

///
auto FindTreeNode(const FlowTree &flow_tree,
                  const std::invocable<const TreeNode &> auto &predicate)
    -> std::optional<const TreeNode *> {
  for (const auto &root : flow_tree.root_nodes) {
    if (const auto found_node = FindTreeNode(root, predicate)) {
      return found_node;
    }
  }

  return std::nullopt;
}

///
auto FindTreeNode(const FlowTree &flow_tree, ne::NodeId node_id)
    -> const TreeNode &;
}  // namespace vh::ponc::flow

#endif  // VH_PONC_FLOW_TREE_TRAVERSAL_H_
