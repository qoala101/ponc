#ifndef VH_DRAW_NEW_LINK_H_
#define VH_DRAW_NEW_LINK_H_

#include <imgui_node_editor.h>

#include <optional>
#include <string>

#include "draw_creation_popup.h"
#include "frame_node.h"

namespace esc::draw {
class Creation {
 public:
  void Draw(coreui::Frame &frame);

 private:
  CreationPopup creation_popup{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_NEW_LINK_H_
