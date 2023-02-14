#ifndef VH_DRAW_REPIN_CURVE_H_
#define VH_DRAW_REPIN_CURVE_H_

#include "draw_i_drawable.h"

namespace esc::draw {
// ---
class RepinCurve : public IDrawable {
 public:
  // ---
  void Draw(State &state) override;
};
}  // namespace esc::draw

#endif  // VH_DRAW_REPIN_CURVE_H_
