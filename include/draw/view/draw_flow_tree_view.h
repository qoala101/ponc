/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_FLOW_TREE_VIEW_H_
#define VH_PONC_DRAW_FLOW_TREE_VIEW_H_

#include <string>
#include <vector>

#include "coreui_flow_tree_node.h"
#include "draw_i_view.h"

namespace vh::ponc::draw {
///
class FlowTreeView : public IView {
 public:
  ///
  auto GetLabel() const -> std::string override;

  ///
  void Draw(const std::vector<coreui::TreeNode>& node_trees);
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_FLOW_TREE_VIEW_H_
