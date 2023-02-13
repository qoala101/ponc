#include "draw_link_popup.h"

#include "cpp_scope.h"
#include "draw_i_family_drawer.h"

namespace esc::draw {
LinkPopup::LinkPopup(core::Link link) : link_{link} {}

auto LinkPopup::GetLabel(State& state) const -> std::string { return "Link"; }

void LinkPopup::DrawContent(State& state) {
  if (ImGui::MenuItem("Delete")) {
    SetVisible(false);

    state.PostEvent([link_id = link_.id](auto& state) {
      State::EraseLink(state, link_id);
    });
  }
}
}  // namespace esc::draw