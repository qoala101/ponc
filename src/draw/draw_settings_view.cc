#include "draw_settings_view.h"

#include "core_tree.h"
#include "cpp_scope.h"
#include "draw_families_view.h"
#include "draw_i_family_drawer.h"
#include "draw_i_node_drawer.h"
#include "esc_state.h"
#include "imgui.h"
#include "imgui_node_editor.h"

namespace esc::draw {
void SettingsView::Draw(State& state) {
  if (!IsVisible()) {
    return;
  }

  {
    const auto window_scope = cpp::Scope{[]() { ImGui::End(); }};

    if (ImGui::Begin("Settings", &GetVisible())) {
      static float color[3] = {};

        ImGui::Text("Color widget:");
        ImGui::SameLine();
        ImGui::ColorEdit3("MyColor##1", (float*)&color);
    }
  }
}
}  // namespace esc::draw