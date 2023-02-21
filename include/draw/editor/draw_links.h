/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_DRAW_LINKS_H_
#define VH_DRAW_LINKS_H_

#include <imgui_node_editor.h>

#include <optional>
#include <string_view>

#include "app_state.h"

namespace esc::draw {
// ---
class Links {
 public:
  // ---
  void Draw(const AppState& app_state);

 private:
  void DrawLinkBeingRepinned(const AppState& app_state);
};
}  // namespace esc::draw

#endif  // VH_DRAW_LINKS_H_
