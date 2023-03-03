#include "draw_i_popup.h"

#include <imgui.h>
#include <imgui_node_editor.h>

#include "cpp_scope_function.h"

namespace ne = ax::NodeEditor;

namespace esc::draw {
///
void IPopup::Show() {
  ImGui::OpenPopup(GetLabel().c_str(), ImGuiPopupFlags_NoOpenOverExistingPopup);
}

///
auto IPopup::DrawContentScope() const -> std::pair<bool, cpp::ScopeFunction> {
  const auto label = GetLabel();
  const auto *label_data = label.c_str();

  ne::Suspend();

  if (ImGui::BeginPopup(label_data)) {
    ImGui::TextUnformatted(label_data);
    ImGui::Separator();

    return {true, cpp::ScopeFunction{[]() {
              ImGui::EndPopup();
              ne::Resume();
            }}};
  }

  return {false, cpp::ScopeFunction{[]() { ne::Resume(); }}};
}
}  // namespace esc::draw