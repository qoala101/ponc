#include "flow_tree.h"

#include <iostream>

#include "cpp_assert.h"

namespace esc::flow {
// ---
auto FindById::operator()(const TreeNode &tree_node) const -> bool {
  return tree_node.node->GetId() == node_id;
}
}  // namespace esc::flow