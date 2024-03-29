/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_settings_view.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <limits>

#include "core_settings.h"
#include "cpp_assert.h"
#include "draw_disable_if.h"
#include "draw_settings_table_row.h"
#include "draw_table_flags.h"
#include "style_default_colors.h"
#include "style_default_sizes.h"
#include "style_flow_colors.h"

namespace vh::ponc::draw {
namespace {
///
void DrawFlowColors(core::Settings& settings) {
  if (ImGui::TreeNodeEx("Flow Colors", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::ColorButton("##Very Low", ImColor{style::FlowColors::kMin});
    ImGui::SameLine();
    ImGui::DragFloat("Very Low", &settings.min_flow, 0.01,
                     -std::numeric_limits<float>::max(), settings.low_flow,
                     "%.2f");

    ImGui::ColorButton("##Low", ImColor{style::FlowColors::kLow});
    ImGui::SameLine();
    ImGui::SliderFloat("Low", &settings.low_flow, settings.min_flow,
                       settings.high_flow, "%.2f");

    ImGui::ColorButton("##Good", ImColor{style::FlowColors::kGood});
    ImGui::SameLine();

    const auto good_flow =
        settings.low_flow + (settings.high_flow - settings.low_flow) / 2;

    ImGui::Text("%.3f Good", good_flow);

    ImGui::ColorButton("##High", ImColor{style::FlowColors::kHigh});
    ImGui::SameLine();
    ImGui::SliderFloat("High", &settings.high_flow, settings.low_flow,
                       settings.max_flow, "%.2f");

    ImGui::ColorButton("##Very High", ImColor{style::FlowColors::kMax});
    ImGui::SameLine();
    ImGui::DragFloat("Very High", &settings.max_flow, 0.01, settings.high_flow,
                     std::numeric_limits<float>::max(), "%.2f");

    ImGui::TreePop();
  }
}

///
void DrawLink(float thickness) {
  auto* draw_list = ImGui::GetWindowDrawList();
  Expects(draw_list != nullptr);

  const auto frame_height = ImGui::GetFrameHeight();
  const auto pos = ImGui::GetCursorScreenPos() + ImVec2{0, frame_height / 2};

  draw_list->AddLine(pos, pos + ImVec2{frame_height, 0},
                     style::DefaultColors::kWhite, thickness);
}

///
void DrawLinkThickness(core::Settings& settings) {
  if (ImGui::TreeNodeEx("Link Thickness", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::Checkbox("Show Length As Thickness", &settings.thick_links);

    const auto frame_height = ImGui::GetFrameHeight();
    const auto dummy_size = ImVec2{frame_height, frame_height};

    {
      const auto disable_scope = EnableIf(settings.thick_links);

      DrawLink(style::DefaultSizes::kMinThickness);
      ImGui::Dummy(dummy_size);
      ImGui::SameLine();
      ImGui::DragFloat("Thin", &settings.min_length, 0.1, 0,
                       settings.max_length, "%.3f");

      DrawLink(style::DefaultSizes::kMaxThickness);
      ImGui::Dummy(dummy_size);
      ImGui::SameLine();
      ImGui::DragFloat("Thick", &settings.max_length, 0.1, settings.min_length,
                       std::numeric_limits<float>::max(), "%.3f");
    }

    ImGui::TreePop();
  }
}

///
void DrawOtherSettings(core::Settings& settings) {
  if (ImGui::TreeNodeEx("Other", ImGuiTreeNodeFlags_DefaultOpen)) {
    if (ImGui::BeginTable("Other", 2, kSettingsTableFlags)) {
      ImGui::TableSetupColumn("Setting", ImGuiTableColumnFlags_NoHeaderLabel);
      ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_NoHeaderLabel);

      DrawSettingsTableRow("Arrange Horizontal Spacing, px");
      ImGui::InputInt("##Arrange Horizontal Spacing, px",
                      &settings.arrange_horizontal_spacing, 0);

      DrawSettingsTableRow("Arrange Vertical Spacing, px");
      ImGui::InputInt("##Arrange Vertical Spacing, px",
                      &settings.arrange_vertical_spacing, 0);

      ImGui::EndTable();
    }

    ImGui::TreePop();
  }
}
}  // namespace

///
auto SettingsView::GetLabel() const -> std::string { return "Settings"; }

///
void SettingsView::Draw(core::Settings& settings) {
  const auto content_scope = DrawContentScope();

  if (!IsOpened()) {
    return;
  }

  DrawFlowColors(settings);
  DrawLinkThickness(settings);

  if (ImGui::TreeNodeEx("Connections", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::TextUnformatted("See View->Connections");
    ImGui::TreePop();
  }

  if (ImGui::TreeNodeEx("Calculator", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::TextUnformatted("See View->Calculator");
    ImGui::TreePop();
  }

  DrawOtherSettings(settings);

  if (ImGui::Button("Reset To Default")) {
    core::Settings::ResetToDefault(settings);
  }
}
}  // namespace vh::ponc::draw