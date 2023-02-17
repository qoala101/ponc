#include "draw_link_popup.h"

#include "app_events.h"
#include "cpp_scope.h"

namespace esc::draw {
void DrawLinkPopup(State& state) {
  if (ImGui::BeginPopup("Link")) {
    const auto popup_scope = cpp::Scope{[]() { ImGui::EndPopup(); }};

    ImGui::TextUnformatted("Link");
    ImGui::Separator();

    if (ImGui::MenuItem("Delete")) {
      state.event_queue->PostEvent(
          event::DeleteLink{.link_id = state.draw_state->popup_link_id});
    }
  }
}
}  // namespace esc::draw