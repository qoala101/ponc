/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_DRAW_NEW_LINK_H_
#define VH_DRAW_NEW_LINK_H_

#include <imgui_node_editor.h>

#include <optional>
#include <string>

#include "draw_new_link_popup.h"
#include "frame_node.h"

namespace esc::draw {
class NewLink {
 public:
  void Draw(frame::Frame &frame);

 private:
  NewLinkPopup new_link_popup{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_NEW_LINK_H_
