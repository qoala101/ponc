#ifndef VH_DRAW_NODE_POPUP_H_
#define VH_DRAW_NODE_POPUP_H_

#include <imgui_node_editor.h>

#include <vector>

#include "coreui_diagram.h"
#include "draw_i_popup.h"

namespace esc::draw {
///
class NodePopup : public IPopup {
 public:
  ///
  void Draw(coreui::Diagram &diagram);
};
}  // namespace esc::draw

#endif  // VH_DRAW_NODE_POPUP_H_
