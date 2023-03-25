#include "draw_settings_view.h"

#include <imgui.h>

#include <limits>

#include "core_settings.h"
#include "draw_id_label.h"

namespace vh::ponc::draw {
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
    ImGui::DragFloat("Very Low", &settings.min_flow, 0.01F,
                     -std::numeric_limits<float>::max(), settings.low_flow,
                     "%.3f");

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
                     std::numeric_limits<float>::max(), "%.3f");

    ImGui::TreePop();
  }
}
}  // namespace vh::ponc::draw