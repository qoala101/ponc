#include "draw_calculator_view.h"

#include "core_i_family.h"
#include "core_project.h"
#include "coreui_calculator.h"
#include "coreui_i_family_traits.h"
#include "draw_disable_if.h"
#include "draw_id_label.h"
#include "draw_settings_table_row.h"
#include "draw_table_flags.h"

namespace vh::ponc::draw {
namespace {
///
void DrawProgressBar(const coreui::Calculator& calculator) {
  if (!calculator.IsRunning()) {
    return;
  }

  ImGui::SameLine();

  const auto progress = calculator.GetProgress();
  const auto label = std::to_string(static_cast<int>(progress * 100)) + "%";

  ImGui::ProgressBar(progress, {-std::numeric_limits<float>::min(), 0},
                     label.c_str());
}

///
void DrawRequirements(core::CalculatorSettings& settings) {
  if (ImGui::CollapsingHeader("Requirements", ImGuiTreeNodeFlags_DefaultOpen)) {
    if (ImGui::BeginTable("Requirements", 2, kSettingsTableFlags)) {
      ImGui::TableSetupColumn("Requirement",
                              ImGuiTableColumnFlags_NoHeaderLabel);
      ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_NoHeaderLabel);

      DrawSettingsTableRow("Min Ouput");
      ImGui::InputFloat("##Min Ouput", &settings.min_output, 0, 0, "%.2f");

      DrawSettingsTableRow("Max Ouput");
      ImGui::InputFloat("##Max Ouput", &settings.max_output, 0, 0, "%.2f");

      DrawSettingsTableRow("Clients");
      ImGui::InputInt("##Clients", &settings.num_clients);

      ImGui::EndTable();
    }
  }
}

///
void DrawFamilySettings(std::string_view label,
                        core::CalculatorFamilySettings& setings) {
  ImGui::TableNextRow();

  ImGui::TableNextColumn();
  ImGui::Checkbox(label.data(), &setings.enabled);

  ImGui::TableNextColumn();
  ImGui::SetNextItemWidth(-std::numeric_limits<float>::min());
  ImGui::InputFloat(IdLabel(setings.family_id).c_str(), &setings.cost, 0, 0,
                    "%.2f");
}

///
void DrawFamilies(core::Project& project) {
  if (ImGui::CollapsingHeader("Node Cost", ImGuiTreeNodeFlags_DefaultOpen)) {
    if (ImGui::BeginTable("Node Cost", 2, kSettingsTableFlags)) {
      ImGui::TableSetupScrollFreeze(0, 1);
      ImGui::TableSetupColumn("Node Type");
      ImGui::TableSetupColumn("Cost, $");
      ImGui::TableHeadersRow();

      for (auto& settings :
           project.GetSettings().calculator_settings.family_settings) {
        const auto& family =
            core::Project::FindFamily(project, settings.family_id);

        DrawFamilySettings(family.CreateUiTraits()->GetLabel(), settings);
      }

      ImGui::EndTable();
    }
  }
}
}  // namespace

///
auto CalculatorView::GetLabel() const -> std::string { return "Calculator"; }

///
void CalculatorView::Draw(coreui::Calculator& calculator,
                          core::Project& project) {
  const auto content_scope = DrawContentScope();

  if (!IsOpened()) {
    return;
  }

  const auto calculation_running = calculator.IsRunning();

  {
    const auto disable_scope = EnableIf(!calculation_running);

    if (ImGui::Button("Calculate")) {
      calculator.Calculate();
    }
  }

  ImGui::SameLine();

  {
    const auto disable_scope = EnableIf(calculation_running);

    if (ImGui::Button("Cancel")) {
      calculator.Cancel();
    }
  }

  DrawProgressBar(calculator);
  DrawRequirements(project.GetSettings().calculator_settings);
  DrawFamilies(project);
}
}  // namespace vh::ponc::draw