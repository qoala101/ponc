#ifndef VH_DRAW_TREE_NODE_H_
#define VH_DRAW_TREE_NODE_H_

#include "coreui_flow_tree.h"

namespace esc::draw {
///
void DrawTreeNode(const coreui::TreeNode& tree_node, bool draw_children = true);
}  // namespace esc::draw

#endif  // VH_DRAW_TREE_NODE_H_
