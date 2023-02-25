#include "draw_node_popup.h"

#include <iostream>

#include "coreui_i_node_traits.h"
#include "cpp_scope.h"
#include "imgui.h"

namespace esc::draw {
void NodePopup::SetNodeId(ne::NodeId node_id) { node_id_ = node_id; }

auto NodePopup::GetLabel() const -> std::string { return "Node"; }

void NodePopup::DrawItems(coreui::Frame& frame) {
  if (ImGui::MenuItem("Delete With Links")) {
    // state.event_queue.PostEvent(
    //     Events::DeleteNodeWithLinks{.node_id = node_id_});
  }

  if (ImGui::MenuItem("Delete")) {
    // state.DeleteNode{.node_id = node_id_});
  }

  if (ImGui::MenuItem("Group")) {
    // auto selected_node_ids = esc::core::Diagram::GetSelectedNodeIds();

    // if (std::ranges::none_of(selected_node_ids,
    //                          [node_id = node_id_](const auto node) {
    //                            return node == node_id;
    //                          })) {
    //   selected_node_ids.emplace_back(node_id_);
    // }

    // state.event_queue.PostEvent(
    //     Events::CreateGroup{.node_ids = std::move(selected_node_ids)});
  }
}
}  // namespace esc::draw