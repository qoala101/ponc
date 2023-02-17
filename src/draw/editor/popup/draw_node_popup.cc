#include "draw_node_popup.h"

#include <iostream>

#include "app_events.h"
#include "coreui_i_node_drawer.h"
#include "cpp_scope.h"
#include "imgui.h"

namespace esc::draw {
void DrawNodePopup(State& state) {
  if (ImGui::BeginPopup("Node")) {
    const auto popup_scope = cpp::Scope{[]() { ImGui::EndPopup(); }};

    ImGui::TextUnformatted("Node");
    ImGui::Separator();

    if (ImGui::MenuItem("Delete With Links")) {
      state.event_queue->PostEvent(event::DeleteNodeWithLinks{
          .node_id = state.draw_state->popup_node_id});
    }

    if (ImGui::MenuItem("Delete")) {
      state.event_queue->PostEvent(
          event::DeleteNode{.node_id = state.draw_state->popup_node_id});
    }

    if (ImGui::MenuItem("Group")) {
      state.event_queue->PostEvent(event::CreateGroup{});
    }
  }
}
}  // namespace esc::draw