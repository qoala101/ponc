#include "draw_link_popup.h"

#include "imgui_node_editor.h"

namespace esc::draw {
void LinkPopup::SetLinkId(ne::LinkId link_id) { link_id_ = link_id; }

auto LinkPopup::GetLabel() const -> std::string { return "Link"; }

void LinkPopup::DrawItems(coreui::Frame &frame) {
  if (ImGui::MenuItem("Delete")) {
    frame.DeleteLink(link_id_);
  }
}
}  // namespace esc::draw