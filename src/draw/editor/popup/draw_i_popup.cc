#include "draw_i_popup.h"

#include <imgui.h>

#include "cpp_scope.h"

namespace esc::draw {
// ---
void IPopup::Show() {
  ImGui::OpenPopup(GetLabel().c_str(), ImGuiPopupFlags_NoOpenOverExistingPopup);
}

// ---
void IPopup::Draw(State &state) {
  const auto label = GetLabel();

  if (ImGui::BeginPopup(label.c_str())) {
    const auto popup_scope = cpp::Scope{[]() { ImGui::EndPopup(); }};

    ImGui::TextUnformatted(label.c_str());
    ImGui::Separator();

    DrawContent(state);
  }
}
}  // namespace esc::draw