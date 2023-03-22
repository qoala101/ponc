#ifndef VH_PONC_DRAW_TREE_NODE_H_
#define VH_PONC_DRAW_TREE_NODE_H_

#include "coreui_flow_tree.h"

namespace vh::ponc::draw {
///
void DrawTreeNode(const coreui::TreeNode& tree_node, bool draw_children = true);
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_TREE_NODE_H_
