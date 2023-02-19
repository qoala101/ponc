#include "draw_settings_view.h"

#include "app_state.h"
#include "coreui_i_family_drawer.h"
#include "coreui_i_node_drawer.h"
#include "cpp_scope.h"
#include "draw_families_view.h"
#include "flow_tree.h"
#include "imgui.h"
#include "imgui_node_editor.h"

namespace esc::draw {
auto SettingsView::GetLabel() const -> std::string { return "Settings"; }

void SettingsView::Draw(AppState& app_state) {
  if (!IsVisible()) {
    return;
  }

  {
    const auto window_scope = cpp::Scope{[]() { ImGui::End(); }};

    if (ImGui::Begin("Settings", &GetVisible())) {
      const auto low_high_mid = state.core_state->flow_colors_.low +
                                (state.core_state->flow_colors_.high -
                                 state.core_state->flow_colors_.low) /
                                    2;

      ImGui::Checkbox("Color Flow", &state.core_state->flow_colors_.color_flow);

      ImGui::ColorButton("MyColor##1",
                         state.core_state->flow_colors_.GetColorForFlowValue(
                             state.core_state->flow_colors_.min));
      ImGui::SameLine();
      ImGui::DragFloat("Very Low##link_color",
                       &state.core_state->flow_colors_.min, 0.01F, -FLT_MAX,
                       state.core_state->flow_colors_.low, "%.3f");

      ImGui::ColorButton("MyColor##2",
                         state.core_state->flow_colors_.GetColorForFlowValue(
                             state.core_state->flow_colors_.low));
      ImGui::SameLine();
      ImGui::SliderFloat("Low##link_color", &state.core_state->flow_colors_.low,
                         state.core_state->flow_colors_.min,
                         state.core_state->flow_colors_.high);

      ImGui::ColorButton(
          "MyColor##5",
          state.core_state->flow_colors_.GetColorForFlowValue(low_high_mid));
      ImGui::SameLine();
      ImGui::Text("%.3f Good", low_high_mid);

      ImGui::ColorButton("MyColor##3",
                         state.core_state->flow_colors_.GetColorForFlowValue(
                             state.core_state->flow_colors_.high));
      ImGui::SameLine();
      ImGui::SliderFloat("High##link_color",
                         &state.core_state->flow_colors_.high,
                         state.core_state->flow_colors_.low,
                         state.core_state->flow_colors_.max);

      ImGui::ColorButton("MyColor##4",
                         state.core_state->flow_colors_.GetColorForFlowValue(
                             state.core_state->flow_colors_.max));
      ImGui::SameLine();
      ImGui::DragFloat("Very High##link_color",
                       &state.core_state->flow_colors_.max, 0.01F,
                       state.core_state->flow_colors_.high, +FLT_MAX, "%.3f");
    }
  }
}
}  // namespace esc::draw