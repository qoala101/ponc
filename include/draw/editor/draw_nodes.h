#ifndef VH_DRAW_NODES_H_
#define VH_DRAW_NODES_H_

#include <memory>

#include "core_i_node.h"
#include "draw_new_link.h"
#include "draw_texture.h"

namespace esc {
class State;
}  // namespace esc

namespace esc::draw {
class Nodes {
 public:
  explicit Nodes(const Texture& node_header_texture,
                 std::shared_ptr<std::optional<NewLink>> new_link);

  void Draw(State& state);

 private:
  void DrawNode(State& state, core::INode& node);

  Texture node_header_texture_{};
  std::shared_ptr<std::optional<NewLink>> new_link_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_NODES_H_
