/**
 * PON Calculator https://github.com/qoala101/ponc
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_TREE_NODE_H_
#define VH_PONC_DRAW_TREE_NODE_H_

#include <functional>

#include "coreui_flow_tree_node.h"

namespace vh::ponc::draw {
///
void DrawInputFlow(const coreui::TreeNode& tree_node);
///
void DrawOutputFlows(const coreui::TreeNode& tree_node);
///
void DrawTreeNode(
    const coreui::TreeNode& tree_node, bool draw_children = true,
    bool selectable = true,
    const std::vector<std::function<void(const coreui::TreeNode&)>>&
        draw_columns = {&DrawInputFlow, &DrawOutputFlows});
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_TREE_NODE_H_
