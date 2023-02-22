/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_DRAW_LINKS_H_
#define VH_DRAW_LINKS_H_

#include "app_state.h"
#include "draw_new_link.h"
#include "draw_nodes.h"

namespace esc::draw {
// ---
class Links {
 public:
  // ---
  void Draw(const AppState &app_state);

 private:
  void DrawLinkBeingRepinned(const core::Diagram &diagram,
                             const NewLink &new_link, const Nodes &nodes);
};
}  // namespace esc::draw

#endif  // VH_DRAW_LINKS_H_
