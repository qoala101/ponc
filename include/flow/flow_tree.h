#ifndef VH_CORE_TREE_H_
#define VH_CORE_TREE_H_

#include <concepts>
#include <functional>
#include <memory>
#include <optional>
#include <vector>

#include "core_i_node.h"
#include "cpp_assert.h"
#include "imgui_node_editor.h"

namespace esc::flow {
struct TreeNode;

// ---
struct TreeNode {
  std::shared_ptr<core::INode> node{};  // change to weak
  std::map<uintptr_t, TreeNode> child_nodes{};
};

// ---
struct Tree {
  std::vector<TreeNode> root_nodes{};
};

// ---
struct FindById {
  // ---
  auto operator()(const TreeNode &tree_node) const -> bool;

  // ---
  ne::NodeId node_id{};
};

// ---
void TraverseDepthFirst(
    const TreeNode &tree_node,
    const std::invocable<const TreeNode &> auto &visitor_before_children,
    const std::invocable<const TreeNode &> auto &visitor_after_children) {
  (*visitor_before_children)(tree_node);

  for (const auto &child : tree_node.child_nodes) {
    TraverseDepthFirst(child.second, visitor_before_children,
                       visitor_after_children);
  }

  (*visitor_after_children)(tree_node);
}

// ---
auto FindTreeNode
    [[nodiscard]] (const TreeNode &tree_node,
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

// ---
auto FindTreeNode
    [[nodiscard]] (const Tree &tree,
                   const std::invocable<const TreeNode &> auto &predicate)
    -> std::optional<const TreeNode *> {
  for (const auto &root : tree.root_nodes) {
    if (const auto found_node = FindTreeNode(root, predicate)) {
      return found_node;
    }
  }

  return std::nullopt;
}
}  // namespace esc::flow

#endif  // VH_CORE_TREE_H_
