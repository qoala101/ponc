#ifndef VH_DRAW_LINK_CONNECTION_PROCESS_H_
#define VH_DRAW_LINK_CONNECTION_PROCESS_H_

#include "draw_i_drawable.h"

namespace esc::draw {
// ---
class LinkConnectionProcess : public IDrawable {
 public:
  // ---
  void Draw(State &state) override;
};
}  // namespace esc::draw

#endif  // VH_DRAW_LINK_CONNECTION_PROCESS_H_
