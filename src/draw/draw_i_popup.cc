#include "draw_i_popup.h"

#include "cpp_scope.h"

namespace esc::draw {
// ---
void IPopup::Draw(State &state) {
  const auto label = GetLabel(state);

  if (ImGui::BeginPopup(label.c_str())) {
    const auto popup_scope = cpp::Scope{[]() { ImGui::EndPopup(); }};

    ImGui::TextUnformatted(label.c_str());
    ImGui::Separator();

    DrawContent(state);
  }
}

// ---
void IPopup::Open(State &state) {
  SetVisible(true);
  ImGui::OpenPopup(GetLabel(state).c_str());
}
}  // namespace esc::draw