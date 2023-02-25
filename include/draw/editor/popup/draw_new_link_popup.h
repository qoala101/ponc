#ifndef VH_DRAW_NEW_LINK_POPUP_H_
#define VH_DRAW_NEW_LINK_POPUP_H_

#include <imgui.h>
#include <imgui_node_editor.h>

#include "draw_i_popup.h"
#include "frame_node.h"

namespace ne = ax::NodeEditor;

namespace esc::draw {
class NewLinkPopup : public IPopup {
 public:
  void SetPosition(const ImVec2 &position);
  void SetDraggedFromPin(ne::PinId pin_id);

 private:
  auto GetLabel() const -> std::string override;
  void DrawItems(frame::Frame &frame) override;

  ImVec2 position_{};
  ne::PinId dragged_from_pin_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_NEW_LINK_POPUP_H_
