#ifndef VH_DRAW_NODES_H_
#define VH_DRAW_NODES_H_

#include "draw_texture.h"

namespace esc {
class State;
}  // namespace esc

namespace esc::draw {
class Nodes {
 public:
  explicit Nodes(const Texture &node_header_texture);

  void Draw(State &state);

 private:
  Texture node_header_texture_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_NODES_H_
