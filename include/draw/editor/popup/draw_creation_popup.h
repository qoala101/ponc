#ifndef VH_DRAW_CREATION_POPUP_H_
#define VH_DRAW_CREATION_POPUP_H_

#include <imgui.h>
#include <imgui_node_editor.h>

#include "draw_i_popup.h"
#include "frame_node.h"

namespace ne = ax::NodeEditor;

namespace esc::draw {
class CreationPopup : public IPopup {
 public:
  void Draw(coreui::Frame &frame);
  void SetPosition(const ImVec2 &position);
  void SetDraggedFromPin(ne::PinId pin_id);

 private:
  auto GetLabel() const -> std::string override;

  ImVec2 position_{};
  ne::PinId dragged_from_pin_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_CREATION_POPUP_H_
