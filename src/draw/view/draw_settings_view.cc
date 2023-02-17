#include "draw_settings_view.h"

#include "app_state.h"
#include "core_tree.h"
#include "coreui_i_family_drawer.h"
#include "coreui_i_node_drawer.h"
#include "cpp_scope.h"
#include "draw_families_view.h"
#include "imgui.h"
#include "imgui_node_editor.h"

namespace esc::draw {
void DrawSettingsView(State& state) {
  if (!state.draw_state->settings_view_visible) {
    return;
  }

  {
    const auto window_scope = cpp::Scope{[]() { ImGui::End(); }};

    if (ImGui::Begin("Settings", &state.draw_state->settings_view_visible)) {
      const auto low_high_mid =
          state.draw_state->low +
          (state.draw_state->high - state.draw_state->low) / 2;

      ImGui::Checkbox("Color Flow", &state.draw_state->color_flow);

      ImGui::ColorButton("MyColor##1", state.draw_state->GetColorForFlowValue(
                                           state.draw_state->min));
      ImGui::SameLine();
      ImGui::DragFloat("Very Low##link_color", &state.draw_state->min, 0.01F,
                       -FLT_MAX, state.draw_state->low, "%.3f");

      ImGui::ColorButton("MyColor##2", state.draw_state->GetColorForFlowValue(
                                           state.draw_state->low));
      ImGui::SameLine();
      ImGui::SliderFloat("Low##link_color", &state.draw_state->low,
                         state.draw_state->min, state.draw_state->high);

      ImGui::ColorButton("MyColor##5",
                         state.draw_state->GetColorForFlowValue(low_high_mid));
      ImGui::SameLine();
      ImGui::Text("%.3f Good", low_high_mid);

      ImGui::ColorButton("MyColor##3", state.draw_state->GetColorForFlowValue(
                                           state.draw_state->high));
      ImGui::SameLine();
      ImGui::SliderFloat("High##link_color", &state.draw_state->high,
                         state.draw_state->low, state.draw_state->max);

      ImGui::ColorButton("MyColor##4", state.draw_state->GetColorForFlowValue(
                                           state.draw_state->max));
      ImGui::SameLine();
      ImGui::DragFloat("Very High##link_color", &state.draw_state->max, 0.01F,
                       state.draw_state->high, +FLT_MAX, "%.3f");
    }
  }
}
}  // namespace esc::draw