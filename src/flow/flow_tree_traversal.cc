/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "flow_tree_traversal.h"

#include "cpp_assert.h"

namespace vh::ponc::flow {
///
auto FindTreeNode(const TreeNode &tree_node, ne::NodeId node_id)
    -> const TreeNode & {
  const auto found_node =
      FindTreeNode(tree_node, [node_id](const auto &tree_node) {
        return tree_node.node_id == node_id;
      });

  Expects(found_node.has_value());
  return **found_node;
}

///
auto FindTreeNode(const std::vector<TreeNode> &flow_trees, ne::NodeId node_id)
    -> const TreeNode & {
  for (const auto &flow_tree : flow_trees) {
    if (const auto found_node =
            FindTreeNode(flow_tree, [node_id](const auto &tree_node) {
              return tree_node.node_id == node_id;
            })) {
      return **found_node;
    }
  }

  Expects(false);
}
}  // namespace vh::ponc::flow