/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#include "draw_i_popup.h"

#include <imgui.h>

#include "app_state.h"
#include "cpp_scope.h"

namespace esc::draw {
// ---
void IPopup::Show() {
  ImGui::OpenPopup(GetLabel().c_str(), ImGuiPopupFlags_NoOpenOverExistingPopup);
}

// ---
void IPopup::Draw(const AppState &app_state) {
  const auto label = GetLabel();
  const auto *label_data = label.c_str();

  if (ImGui::BeginPopup(label_data)) {
    const auto popup_scope = cpp::Scope{[]() { ImGui::EndPopup(); }};

    ImGui::TextUnformatted(label_data);
    ImGui::Separator();

    DrawItems(app_state);
  }
}
}  // namespace esc::draw