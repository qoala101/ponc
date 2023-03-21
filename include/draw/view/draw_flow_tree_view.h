#ifndef VH_DRAW_FLOW_TREE_VIEW_H_
#define VH_DRAW_FLOW_TREE_VIEW_H_

#include <string>

#include "coreui_flow_tree.h"
#include "draw_i_view.h"

namespace esc::draw {
///
class FlowTreeView : public IView {
 public:
  ///
  auto GetLabel() const -> std::string override;

  ///
  void Draw(const coreui::FlowTree& flow_tree);
};
}  // namespace esc::draw

#endif  // VH_DRAW_FLOW_TREE_VIEW_H_
