#include "draw_link_popup.h"

#include "app_events.h"
#include "imgui_node_editor.h"

namespace esc::draw {
void LinkPopup::SetLinkId(ne::LinkId link_id) { link_id_ = link_id; }

auto LinkPopup::GetLabel() const -> std::string { return "Link"; }

void LinkPopup::DrawContent(State& state) {
  if (ImGui::MenuItem("Delete")) {
    state.event_queue->PostEvent(event::DeleteLink{.link_id = link_id_});
  }
}
}  // namespace esc::draw