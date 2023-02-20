/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_DRAW_NODES_H_
#define VH_DRAW_NODES_H_

#include <memory>

#include "app_state.h"
#include "core_i_node.h"
#include "draw_texture.h"
#include "flow_node_flow.h"

namespace esc::draw {
// ---
class Nodes {
 public:
  // ---
  explicit Nodes(const Texture& node_header_texture);

  // ---
  void Draw(const AppState& app_state);

 private:
  // ---
  void DrawNode(core::INode& node, flow::NodeFlow& node_flow);

  // ---
  Texture node_header_texture_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_NODES_H_
