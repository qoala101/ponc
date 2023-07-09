/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_connections_view.h"

#include <imgui.h>

#include <optional>
#include <string>
#include <vector>

#include "core_diagram.h"
#include "core_project.h"
#include "coreui_diagram.h"
#include "cpp_assert.h"
#include "draw_disable_if.h"
#include "draw_id_label.h"
#include "draw_rename_widget.h"

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

  ImGui::BeginHorizontal("Controls");

  if (ImGui::Button("Create")) {
    project.AddConnection();
  }

  if (ImGui::Button("Rename")) {
    selected_action = Action::kStartRenaming;
    focus_name_input_ = true;
    ImGui::OpenPopup("Rename Connection");
  }

  if (const auto rename_confirmed = DrawRenamePopup()) {
    selected_action = Action::kConfirmRename;
  }

  const auto& connections = core_project.GetConnections();

  {
    const auto disable_scope = DisableIf(connections.size() <= 1);

    if (ImGui::Button("Delete")) {
      selected_action = Action::kDelete;
    }
  }

  ImGui::EndHorizontal();

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
                                  int connection_index, Action action) {
  Expects(connection_index >= 0);

  auto& diagrams = project.GetProject().GetDiagrams();
  Expects(connection_index < static_cast<int>(diagrams.size()));

  auto& diagram = diagrams[connection_index];

  switch (action) {
    case Action::kStartRenaming: {
      rename_buffer_.Set(diagram.GetName());
      break;
    }
    case Action::kConfirmRename:
      diagram.SetName(rename_buffer_.AsTrimmed());
      break;
    case Action::kDelete:
      project.DeleteDiagram(connection_index);
      break;
  }
}

///
void ConnectionsView::DrawDiagrams(coreui::Project& project,
                                   std::optional<Action> selected_action) {
  // NOLINTBEGIN(*-signed-bitwise)
  const auto table_flags = ImGuiTableFlags_BordersH |
                           ImGuiTableFlags_BordersOuter |
                           ImGuiTableFlags_ScrollY;
  // NOLINTEND(*-signed-bitwise)

  if (ImGui::BeginTable("Connections", 1, table_flags)) {
    auto& connections = project.GetProject().GetConnections();

    for (auto i = 0; i < static_cast<int>(connections.size()); ++i) {
      ImGui::TableNextRow();
      ImGui::TableNextColumn();

      auto& connection = connections[i];
      const auto item_is_selected = false;
      // &connection == &project.GetDiagram().GetDiagram();

      if (ImGui::Selectable(IdLabel(i, connection.name).c_str(),
                            item_is_selected) &&
          !item_is_selected) {
        project.SetDiagram(i);
      }

      if (item_is_selected && selected_action.has_value()) {
        ApplyAction(project, i, *selected_action);
      }
    }

    ImGui::EndTable();
  }
}
}  // namespace vh::ponc::draw