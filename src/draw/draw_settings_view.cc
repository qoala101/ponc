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
void DrawSettingsView(State& state) {
  if (!state.DRAW_.settings_view_visible) {
    return;
  }

  {
    const auto window_scope = cpp::Scope{[]() { ImGui::End(); }};

    if (ImGui::Begin("Settings", &state.DRAW_.settings_view_visible)) {
      const auto low_high_mid =
          state.drawing_.link_colors.low +
          (state.drawing_.link_colors.high - state.drawing_.link_colors.low) / 2;

      ImGui::Checkbox("Color Flow", &state.drawing_.link_colors.color_flow);

      ImGui::ColorButton("MyColor##1", state.GetColorForFlowValue(
                                           state.drawing_.link_colors.min));
      ImGui::SameLine();
      ImGui::DragFloat("Very Low##link_color", &state.drawing_.link_colors.min,
                       0.01F, -FLT_MAX, state.drawing_.link_colors.low, "%.3f");

      ImGui::ColorButton("MyColor##2", state.GetColorForFlowValue(
                                           state.drawing_.link_colors.low));
      ImGui::SameLine();
      ImGui::SliderFloat("Low##link_color", &state.drawing_.link_colors.low,
                         state.drawing_.link_colors.min,
                         state.drawing_.link_colors.high);

      ImGui::ColorButton("MyColor##5", state.GetColorForFlowValue(low_high_mid));
      ImGui::SameLine();
      ImGui::Text("%.3f Good", low_high_mid);

      ImGui::ColorButton("MyColor##3", state.GetColorForFlowValue(
                                           state.drawing_.link_colors.high));
      ImGui::SameLine();
      ImGui::SliderFloat("High##link_color", &state.drawing_.link_colors.high,
                         state.drawing_.link_colors.low,
                         state.drawing_.link_colors.max);

      ImGui::ColorButton("MyColor##4", state.GetColorForFlowValue(
                                           state.drawing_.link_colors.max));
      ImGui::SameLine();
      ImGui::DragFloat("Very High##link_color", &state.drawing_.link_colors.max,
                       0.01F, state.drawing_.link_colors.high, +FLT_MAX,
                       "%.3f");
    }
  }
}
}  // namespace esc::draw