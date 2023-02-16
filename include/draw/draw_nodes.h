#ifndef VH_DRAW_NODES_H_
#define VH_DRAW_NODES_H_

#include "draw_i_drawable.h"
#include "esc_textures_handle.h"

namespace esc::draw {
// ---
class Nodes : public IDrawable {
 public:
  // explicit Nodes(esc::TexturesHandle textures);

  // ---
  void Draw(State &state) override;

 private:
  // TexturesHandle textures_;
};
}  // namespace esc::draw

#endif  // VH_DRAW_NODES_H_
