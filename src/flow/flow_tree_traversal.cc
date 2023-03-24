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
auto FindTreeNode(const FlowTree &flow_tree, ne::NodeId node_id)
    -> const TreeNode & {
  for (const auto &root : flow_tree.root_nodes) {
    if (const auto found_node =
            FindTreeNode(root, [node_id](const auto &tree_node) {
              return tree_node.node_id == node_id;
            })) {
      return **found_node;
    }
  }

  Expects(false);
}
}  // namespace vh::ponc::flow