#include "draw_link_popup.h"

#include "app_events.h"
#include "imgui_node_editor.h"

namespace esc::draw {
void LinkPopup::SetLinkId(ne::LinkId link_id) { link_id_ = link_id; }

auto LinkPopup::GetLabel() const -> std::string { return "Link"; }

void LinkPopup::DrawItems(const AppState& app_state) {
  if (ImGui::MenuItem("Delete")) {
    // app_state.event_queue.PostEvent(Events::DeleteLink{.link_id =
    // link_id_});
  }
}
}  // namespace esc::draw