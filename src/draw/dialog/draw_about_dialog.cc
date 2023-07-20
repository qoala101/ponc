/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_about_dialog.h"

#include <imgui.h>

#include "cpp_assert.h"
#include "draw_settings_table_row.h"

namespace vh::ponc::draw {
///
void AboutDialog::Open() { open_requested_ = true; }

///
void AboutDialog::Draw() {
  if (open_requested_) {
    ImGui::OpenPopup("About");
  }

  const auto *viewport = ImGui::GetMainViewport();
  Expects(viewport != nullptr);

  ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Appearing,
                          ImVec2{0.5, 0.5});

  if (ImGui::BeginPopupModal("About", &open_requested_,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::TextUnformatted("Passive Optical Network Calculator 0.5.4");
    ImGui::Separator();

    if (ImGui::BeginTable("About", 2)) {
      ImGui::TableSetupColumn("Field", ImGuiTableColumnFlags_NoHeaderLabel);
      ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_NoHeaderLabel);

      DrawSettingsTableRow("Author");
      ImGui::TextUnformatted("Volodymyr Hromakov");

      DrawSettingsTableRow("Email");
      ImGui::TextUnformatted("4y5t6r@gmail.com");

      DrawSettingsTableRow("Site");
      ImGui::TextUnformatted("https://github.com/qoala101/ponc");

      DrawSettingsTableRow("Date");
      ImGui::TextUnformatted("July 23, 2023");

      DrawSettingsTableRow("License");
      ImGui::TextUnformatted("MIT");

      ImGui::EndTable();
    }
    ImGui::EndPopup();
  }
}
}  // namespace vh::ponc::draw