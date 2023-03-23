#include "flow_tree_traversal.h"

namespace vh::ponc::flow {
///
auto FindById::operator()(const TreeNode &tree_node) const -> bool {
  return tree_node.node_id == node_id;
}
}  // namespace vh::ponc::flow