#include "flow_tree.h"

#include <iostream>

#include "cpp_assert.h"

namespace esc::flow {
void TraverseDepthFirst(const TreeNode &tree_node,
                        const std::optional<Visitor> &visit_before_children,
                        const std::optional<Visitor> &visit_after_children) {
  Expects(visit_before_children.has_value() ||
          visit_after_children.has_value());

  if (visit_before_children.has_value()) {
    (*visit_before_children)(tree_node);
  }

  for (const auto &child : tree_node.child_nodes) {
    TraverseDepthFirst(child.second, visit_before_children,
                       visit_after_children);
  }

  if (!visit_after_children.has_value()) {
    return;
  }

  (*visit_after_children)(tree_node);
}
}  // namespace esc::flow