#ifndef VH_DRAW_CREATE_NODE_POPUP_H_
#define VH_DRAW_CREATE_NODE_POPUP_H_

#include <imgui.h>
#include <imgui_node_editor.h>

#include <memory>

#include "coreui_diagram.h"
#include "draw_i_popup.h"

namespace esc::draw {
///
class CreateNodePopup : public IPopup {
 public:
  ///
  void Draw(coreui::Diagram &diagram);
  ///
  void SetPos(const ImVec2 &pos);

 private:
  ///
  ImVec2 pos_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_CREATE_NODE_POPUP_H_
