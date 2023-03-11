#include "draw_node_popup.h"

#include "imgui.h"

namespace esc::draw {
///
void NodePopup::Draw(const Callbacks& callbacks) {
  const auto content_scope = DrawContentScope();

  if (!IsOpened()) {
    return;
  }

  if (ImGui::MenuItem("Delete")) {
    callbacks.node_deleted(node_id_);
  }

  if (ImGui::MenuItem("Delete With Links")) {
    callbacks.node_deleted_with_links(node_id_);
  }

  if (ImGui::BeginMenu("Replace With")) {
    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("Fill With")) {
    ImGui::EndMenu();
  }
}

///
void NodePopup::SetNodeId(ne::NodeId node_id) { node_id_ = node_id; }

///
auto NodePopup::GetLabel() const -> std::string { return "Node"; }
}  // namespace esc::draw