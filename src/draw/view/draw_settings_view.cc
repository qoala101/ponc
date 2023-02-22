#include "draw_settings_view.h"

#include <imgui.h>

#include "app_state.h"
#include "core_project.h"
#include "core_settings.h"
#include "cpp_scope.h"

namespace esc::draw {
auto SettingsView::GetLabel() const -> std::string { return "Settings"; }

void SettingsView::Draw(const AppState& app_state) {
  if (!IsVisible()) {
    return;
  }

  auto& settings = app_state.project.GetSettings();

  {
    const auto window_scope = cpp::Scope{[]() { ImGui::End(); }};

    if (ImGui::Begin("Settings", &GetVisible())) {
      const auto low_high_mid =
          settings.low + (settings.high - settings.low) / 2;

      ImGui::Checkbox("Color Flow", &color_flow);

      ImGui::ColorButton("MyColor##1",
                         GetColorForFlowValue(settings.min, settings));
      ImGui::SameLine();
      ImGui::DragFloat("Very Low##link_color", &settings.min, 0.01F, -FLT_MAX,
                       settings.low, "%.3f");

      ImGui::ColorButton("MyColor##2",
                         GetColorForFlowValue(settings.low, settings));
      ImGui::SameLine();
      ImGui::SliderFloat("Low##link_color", &settings.low, settings.min,
                         settings.high);

      ImGui::ColorButton("MyColor##5",
                         GetColorForFlowValue(low_high_mid, settings));
      ImGui::SameLine();
      ImGui::Text("%.3f Good", low_high_mid);

      ImGui::ColorButton("MyColor##3",
                         GetColorForFlowValue(settings.high, settings));
      ImGui::SameLine();
      ImGui::SliderFloat("High##link_color", &settings.high, settings.low,
                         settings.max);

      ImGui::ColorButton("MyColor##4",
                         GetColorForFlowValue(settings.max, settings));
      ImGui::SameLine();
      ImGui::DragFloat("Very High##link_color", &settings.max, 0.01F,
                       settings.high, +FLT_MAX, "%.3f");
    }
  }
}

auto SettingsView::GetColorForFlowValue(float value,
                                        const core::Settings& settings) const
    -> ImColor {
  if (!color_flow) {
    return ImColor{1.F, 1.F, 1.F};
  }

  const auto blue = ImColor{0.F, 0.F, 1.F};
  const auto red = ImColor{1.F, 0.F, 0.F};

  if (value < settings.min) {
    return blue;
  }

  if (value >= settings.max) {
    return red;
  }

  const auto range = (settings.max - settings.min);
  const auto value_percentage = (value - settings.min) / range;
  const auto low_percentage = (settings.low - settings.min) / range;
  const auto high_percentage = (settings.high - settings.min) / range;

  auto percentage = 0.0F;
  auto start_color = ImColor{};
  auto end_color = ImColor{};

  const auto blue_green = ImColor{0.F, 1.F, 1.F};
  const auto green = ImColor{0.F, 1.F, 0.F};
  const auto green_red = ImColor{1.F, 1.F, 0.F};

  if (value_percentage < low_percentage) {
    percentage = value_percentage / low_percentage;
    start_color = blue;
    end_color = blue_green;
  } else if (value_percentage >= high_percentage) {
    percentage =
        (value_percentage - high_percentage) / (1.0F - high_percentage);
    start_color = green_red;
    end_color = red;
  } else {
    const auto low_high_range = (high_percentage - low_percentage);
    percentage = (value_percentage - low_percentage) / low_high_range;

    if (percentage < 0.5F) {
      percentage = percentage * 2;
      start_color = blue_green;
      end_color = green;
    } else {
      percentage = (percentage - 0.5F) * 2;
      start_color = green;
      end_color = green_red;
    }
  }

  return ImColor{start_color.Value.x +
                     percentage * (end_color.Value.x - start_color.Value.x),
                 start_color.Value.y +
                     percentage * (end_color.Value.y - start_color.Value.y),
                 start_color.Value.z +
                     percentage * (end_color.Value.z - start_color.Value.z)};
}
}  // namespace esc::draw