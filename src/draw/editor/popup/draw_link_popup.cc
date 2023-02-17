#include "draw_link_popup.h"

#include "cpp_scope.h"

namespace esc::draw {
void DrawLinkPopup(State& state) {
  if (ImGui::BeginPopup("Link")) {
    const auto popup_scope = cpp::Scope{[]() { ImGui::EndPopup(); }};

    ImGui::TextUnformatted("Link");
    ImGui::Separator();

    if (ImGui::MenuItem("Delete")) {
      state.PostEvent([](auto& state) {
        State::EraseLink(state, state.DRAW_.popup_link_id);
      });
    }
  }
}
}  // namespace esc::draw