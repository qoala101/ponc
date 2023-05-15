#ifndef VH_PONC_DRAW_FLOW_TREE_VIEW_H_
#define VH_PONC_DRAW_FLOW_TREE_VIEW_H_

#include <string>

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
