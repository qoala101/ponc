#include "draw_settings_view.h"

#include <imgui.h>

#include "core_settings.h"
#include "draw_id_label.h"

namespace esc::draw {
///
auto SettingsView::GetLabel() const -> std::string { return "Settings"; }

///
void SettingsView::Draw(core::Settings& settings) {
  const auto content_scope = DrawContentScope();

  if (!IsOpened()) {
    return;
  }

  if (ImGui::TreeNodeEx("Flow Colors", ImGuiTreeNodeFlags_DefaultOpen)) {
    auto color_button_id = 0;

    ImGui::ColorButton(IdLabel(color_button_id++).c_str(),
                       ImColor{0.F, 0.F, 1.F});
    ImGui::SameLine();
    ImGui::DragFloat("Very Low", &settings.min_flow, 0.01F, -FLT_MAX,
                     settings.low_flow, "%.3f");

    ImGui::ColorButton(IdLabel(color_button_id++).c_str(),
                       ImColor{0.F, 1.F, 1.F});
    ImGui::SameLine();
    ImGui::SliderFloat("Low", &settings.low_flow, settings.min_flow,
                       settings.high_flow);

    ImGui::ColorButton(IdLabel(color_button_id++).c_str(),
                       ImColor{0.F, 1.F, 0.F});
    ImGui::SameLine();

    const auto good_flow =
        settings.low_flow + (settings.high_flow - settings.low_flow) / 2;

    ImGui::Text("%.3f Good", good_flow);

    ImGui::ColorButton(IdLabel(color_button_id++).c_str(),
                       ImColor{1.F, 1.F, 0.F});
    ImGui::SameLine();
    ImGui::SliderFloat("High", &settings.high_flow, settings.low_flow,
                       settings.max_flow);

    ImGui::ColorButton(IdLabel(color_button_id++).c_str(),
                       ImColor{1.F, 0.F, 0.F});
    ImGui::SameLine();
    ImGui::DragFloat("Very High", &settings.max_flow, 0.01F, settings.high_flow,
                     +FLT_MAX, "%.3f");

    ImGui::TreePop();
  }
}
}  // namespace esc::draw