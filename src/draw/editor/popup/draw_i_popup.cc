#include "draw_i_popup.h"

#include <imgui.h>

#include "cpp_scope.h"
#include "frame_node.h"

namespace esc::draw {
void IPopup::Show() {
  ImGui::OpenPopup(GetLabel().c_str(), ImGuiPopupFlags_NoOpenOverExistingPopup);
}

void IPopup::Draw(frame::Frame &frame) {
  const auto label = GetLabel();
  const auto *label_data = label.c_str();

  if (ImGui::BeginPopup(label_data)) {
    const auto popup_scope = cpp::Scope{[]() { ImGui::EndPopup(); }};

    ImGui::TextUnformatted(label_data);
    ImGui::Separator();

    DrawItems(frame);
  }
}
}  // namespace esc::draw