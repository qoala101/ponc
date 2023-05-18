/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_NODE_H_
#define VH_PONC_DRAW_NODE_H_

#include "coreui_node.h"
#include "coreui_node_mover.h"

namespace vh::ponc::draw {
///
void DrawNode(coreui::Node &node, coreui::NodeMover &node_mover);
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_NODE_H_
