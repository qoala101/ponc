#ifndef VH_DRAW_NODE_FACTORIES_VIEW_H_
#define VH_DRAW_NODE_FACTORIES_VIEW_H_

#include "draw_i_drawable.h"

namespace esc::draw {
// ---
class NodeFactoriesView : public IDrawable {
 public:
  // ---
  void Draw(AppState &app_state) override;
};
}  // namespace esc::draw

#endif  // VH_DRAW_NODE_FACTORIES_VIEW_H_
