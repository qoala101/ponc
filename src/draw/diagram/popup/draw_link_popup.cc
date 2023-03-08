#include "draw_link_popup.h"

#include "imgui_node_editor.h"

namespace esc::draw {
///
void LinkPopup::Draw(const Callbacks &callbacks) {
  const auto content_scope = DrawContentScope();

  if (!IsOpened()) {
    return;
  }

  if (ImGui::MenuItem("Delete")) {
    callbacks.link_deleted(link_id_);
  }
}

///
void LinkPopup::SetLinkId(ne::LinkId link_id) { link_id_ = link_id; }

///
auto LinkPopup::GetLabel() const -> std::string { return "Link"; }
}  // namespace esc::draw