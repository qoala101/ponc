#ifndef VH_DRAW_GROUPS_H_
#define VH_DRAW_GROUPS_H_

#include "draw_i_drawable.h"

namespace esc::draw {
// ---
class Groups : public IDrawable {
 public:
  // ---
  void Draw(State &state) override;
};
}  // namespace esc::draw

#endif  // VH_DRAW_GROUPS_H_
