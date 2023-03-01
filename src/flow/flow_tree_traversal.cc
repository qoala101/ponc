#include "flow_tree_traversal.h"

namespace esc::flow {
///
auto FindById::operator()(const TreeNode &tree_node) const -> bool {
  return tree_node.node->GetId() == node_id;
}
}  // namespace esc::flow