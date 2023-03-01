#include "draw_link_popup.h"

#include "imgui_node_editor.h"

namespace esc::draw {
void LinkPopup::SetLinkId(ne::LinkId link_id) { link_id_ = link_id; }

auto LinkPopup::GetLabel() const -> std::string { return "Link"; }

void LinkPopup::Draw(const SignalDeleteLink &signal_delete_link) {
  const auto [is_visible, content_scope] = DrawContentScope();

  if (!is_visible) {
    return;
  }

  if (ImGui::MenuItem("Delete")) {
    signal_delete_link(link_id_);
  }
}
}  // namespace esc::draw