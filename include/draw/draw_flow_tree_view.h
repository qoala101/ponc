#ifndef VH_DRAW_FLOW_TREE_VIEW_H_
#define VH_DRAW_FLOW_TREE_VIEW_H_

#include "draw_i_drawable.h"

namespace esc::draw {
// ---
class FlowTreeView : public IDrawable {
 public:
  // ---
  void Draw(State &state) override;
};
}  // namespace esc::draw

#endif  // VH_DRAW_FLOW_TREE_VIEW_H_
