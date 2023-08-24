/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_connections_view.h"

#include <imgui.h>
#include <imgui_node_editor.h>

#include <limits>
#include <optional>
#include <string>
#include <vector>

#include "core_connection.h"
#include "core_project.h"
#include "core_settings.h"
#include "draw_disable_if.h"
#include "draw_help_marker.h"
#include "draw_rename_widget.h"
#include "draw_table_flags.h"
#include "style_default_colors.h"

namespace vh::ponc::draw {
///
auto ConnectionsView::GetLabel() const -> std::string { return "Connections"; }

///
auto ConnectionsView::DrawRenamePopup() {
  auto rename_confirmed = false;

  if (ImGui::BeginPopup("Rename Connection")) {
    if (focus_name_input_) {
      ImGui::SetKeyboardFocusHere();
    }

    rename_confirmed = DrawRenameWidget("##Connection Name", rename_buffer_);
    ImGui::EndPopup();
  }

  focus_name_input_ = false;
  return rename_confirmed;
}

///
auto ConnectionsView::DrawControls(coreui::Project& project) {
  const auto& core_project = project.GetProject();
  auto selected_action = std::optional<Action>{};

  // ImGui::BeginHorizontal("Controls");

  if (ImGui::Button("Create")) {
    project.AddConnection();
  }

  const auto custom_connection =
      core_project.GetSettings().default_connection.has_value();

  {
    const auto disable_scope = EnableIf(custom_connection);

    if (ImGui::Button("Rename")) {
      selected_action = Action::kStartRenaming;
      focus_name_input_ = true;
      ImGui::OpenPopup("Rename Connection");
    }

    if (const auto rename_confirmed = DrawRenamePopup()) {
      selected_action = Action::kConfirmRename;
    }

    if (ImGui::Button("Delete")) {
      selected_action = Action::kDelete;
    }
  }

  DrawHelpMarker("Selected connection is used for new links");
  // ImGui::EndHorizontal();

  return selected_action;
}

///
void ConnectionsView::Draw(coreui::Project& project) {
  const auto content_scope = DrawContentScope();

  if (!IsOpened()) {
    return;
  }

  const auto control_action = DrawControls(project);
  DrawDiagrams(project, control_action);
}

///
void ConnectionsView::ApplyAction(coreui::Project& project,
                                  core::Connection& connection, Action action) {
  switch (action) {
    case Action::kStartRenaming: {
      rename_buffer_.Set(connection.name);
      break;
    }

    case Action::kConfirmRename:
      connection.name = rename_buffer_.AsTrimmed();
      break;

    case Action::kDelete:
      project.DeleteConnection(connection.id);
      break;
  }
}

///
void ConnectionsView::DrawDiagrams(coreui::Project& project,
                                   std::optional<Action> selected_action) {
  if (ImGui::BeginTable("Connections", 3, kExpandingTableFlags)) {
    ImGui::TableSetupScrollFreeze(0, 1);
    ImGui::TableSetupColumn("Name");
    ImGui::TableSetupColumn("Attenuation/Length");
    ImGui::TableSetupColumn("Attenuation Added");
    ImGui::TableHeadersRow();

    auto& default_connection =
        project.GetProject().GetSettings().default_connection;

    ImGui::TableNextRow();

    ImGui::TableNextColumn();
    ImGui::ColorButton("##Color", ImColor{style::DefaultColors::kWhite},
                       ImGuiColorEditFlags_NoAlpha);
    ImGui::SameLine();

    if (ImGui::Selectable("<None>", !default_connection.has_value())) {
      default_connection.reset();
    }

    auto& connections = project.GetProject().GetConnections();

    for (auto& connection : connections) {
      ImGui::PushID(connection.id.AsPointer());
      ImGui::TableNextRow();

      ImGui::TableNextColumn();
      ImGui::ColorEdit3(
          "##Color", &connection.color.Value.x,
          ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
      ImGui::SameLine();

      const auto item_is_selected = default_connection.has_value() &&
                                    (connection.id == *default_connection);

      if (ImGui::Selectable(connection.name.c_str(), item_is_selected)) {
        default_connection = connection.id;
      }

      if (item_is_selected && selected_action.has_value()) {
        ApplyAction(project, connection, *selected_action);
      }

      ImGui::TableNextColumn();
      ImGui::SetNextItemWidth(-std::numeric_limits<float>::min());
      ImGui::InputFloat("##Attenuation/Length", &connection.drop_per_length, 0,
                        0, "%.2f");

      ImGui::TableNextColumn();
      ImGui::SetNextItemWidth(-std::numeric_limits<float>::min());
      ImGui::InputFloat("##Attenuation Added", &connection.drop_added, 0, 0,
                        "%.2f");

      ImGui::PopID();
    }

    ImGui::EndTable();
  }
}
}  // namespace vh::ponc::draw