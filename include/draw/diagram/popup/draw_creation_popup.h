#ifndef VH_DRAW_CREATION_POPUP_H_
#define VH_DRAW_CREATION_POPUP_H_

#include <imgui.h>
#include <imgui_node_editor.h>

#include "coreui_project.h"
#include "draw_i_popup.h"

namespace ne = ax::NodeEditor;

namespace esc::draw {
class CreationPopup : public IPopup {
 public:
  void Draw(coreui::Project &frame);
  void SetPos(const ImVec2 &pos);
  void SetDraggedFromPin(ne::PinId pin_id);

 private:
  auto GetLabel() const -> std::string override;

  ImVec2 pos_{};
  ne::PinId dragged_from_pin_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_CREATION_POPUP_H_
