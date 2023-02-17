#ifndef VH_DRAW_FLOW_TREE_VIEW_H_
#define VH_DRAW_FLOW_TREE_VIEW_H_

#include <string>

#include "draw_i_hidable.h"

namespace esc {
class State;
}  // namespace esc

namespace esc::draw {
class FlowTreeView : public IHidable {
 public:
  auto GetLabel [[nodiscard]] () const -> std::string;
  void Draw(State &state);
};
}  // namespace esc::draw

#endif  // VH_DRAW_FLOW_TREE_VIEW_H_
