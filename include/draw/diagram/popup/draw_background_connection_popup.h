#ifndef VH_DRAW_BACKGROUND_CONNECTION_POPUP_H_
#define VH_DRAW_BACKGROUND_CONNECTION_POPUP_H_

#include <imgui.h>
#include <imgui_node_editor.h>

#include "coreui_family.h"
#include "draw_i_popup.h"

namespace ne = ax::NodeEditor;

namespace esc::draw {
///
class BackgroundConnectionPopup : public IPopup {
 public:
  ///
  void Draw(const std::vector<coreui::FamilyGroup> &family_groups);
  ///
  void SetPos(const ImVec2 &pos);
  ///
  void SetSourcePin(ne::PinId pin_id, ne::PinKind pin_kind);

 private:
  ///
  auto GetLabel() const -> std::string override;
  ///
  auto GetGroupsValidForConnection(
      const std::vector<coreui::FamilyGroup> &family_groups) const;

  ///
  ImVec2 pos_{};
  ///
  ne::PinId source_pin_id_{};
  ///
  ne::PinKind source_pin_kind_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_BACKGROUND_CONNECTION_POPUP_H_
