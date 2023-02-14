#ifndef VH_DRAW_LINKS_H_
#define VH_DRAW_LINKS_H_

#include "draw_i_drawable.h"

namespace esc::draw {
// ---
class Links : public IDrawable {
 public:
  // ---
  void Draw(State &state) override;
};
}  // namespace esc::draw

#endif  // VH_DRAW_LINKS_H_
