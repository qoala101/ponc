#include "draw_node_popup.h"

#include "imgui.h"

namespace esc::draw {
///
void NodePopup::Draw(const Callbacks &callbacks) {
  const auto content_scope = DrawContentScope();

  if (!IsOpened()) {
    return;
  }

  if (ImGui::MenuItem("Delete")) {
    callbacks.node_deleted(node_id_);
  }
}

///
void NodePopup::SetNodeId(ne::NodeId node_id) { node_id_ = node_id; }

///
auto NodePopup::GetLabel() const -> std::string { return "Node"; }
}  // namespace esc::draw