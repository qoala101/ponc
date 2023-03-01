#include "draw_main_menu_bar.h"

#include <imgui.h>

#include <functional>

#include "core_project.h"
#include "coreui_project.h"
#include "cpp_scope.h"
#include "draw_main_window.h"
#include "draw_open_file_dialog.h"
#include "draw_save_as_file_dialog.h"
#include "draw_settings_view.h"

namespace esc::draw {
namespace {
auto AsLowerCase(std::string text) {
  for (auto &character : text) {
    character = static_cast<char>(std::tolower(character));
  }

  return text;
}

void DrawViewMenuItem(auto &view) {
  const auto visible = view.IsVisible();

  if (ImGui::MenuItem(view.GetLabel().c_str(), nullptr, visible)) {
    view.SetVisible(!visible);
  }
}
}  // namespace

void MainMenuBar::Draw(coreui::Project &frame) {
  if (ImGui::BeginMainMenuBar()) {
    DrawFileMenu(frame);
    DrawViewsMenu(frame);

    if (ImGui::MenuItem("Zoom to Content")) {
      ne::NavigateToContent();
    }

    if (ImGui::MenuItem("Show Flow")) {
      for (const auto &link : frame.GetProject().GetDiagram().GetLinks()) {
        ne::Flow(link.id);
      }
    }

    ImGui::EndMainMenuBar();
  }
}

void MainMenuBar::DrawFileMenu(coreui::Project &frame) {
  if (ImGui::BeginMenu("File")) {
    if (ImGui::MenuItem("Open...", nullptr)) {
      open_file_dialog.Show();
    }

    if (ImGui::MenuItem("Save As...")) {
      save_as_file_dialog.Show();
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Reset")) {
      // frame.ResetDiagram{});
    }

    ImGui::EndMenu();
  }

  open_file_dialog.Draw(
      std::bind_front(&MainMenuBar::SlotOpenFileSelected, frame));
  save_as_file_dialog.Draw(
      std::bind_front(&MainMenuBar::SlotSaveAsFileSelected, frame));
}

void MainMenuBar::DrawViewsMenu(coreui::Project &frame) {
  if (ImGui::BeginMenu("Views")) {
    // DrawViewMenuItem(state.draw_state->families_view_);
    // DrawViewMenuItem(state.draw_state->flow_tree_view_);
    // DrawViewMenuItem(state.draw_state->groups_view_);
    // DrawViewMenuItem(state.draw_state->group_settings_view_);
    DrawViewMenuItem(settings_view);
    ImGui::EndMenu();
  }

  // state.draw_state->families_view_.Draw(state);
  // state.draw_state->flow_tree_view_.Draw(state);
  // state.draw_state->groups_view_.Draw(state);
  // state.draw_state->group_settings_view_.Draw(state);
  settings_view.Draw(frame.GetProject().GetSettings());
}

void MainMenuBar::SlotOpenFileSelected(coreui::Project &frame,
                                       std::string file_path) {
  frame.OpenFromFile(std::move(file_path));
}

void MainMenuBar::SlotSaveAsFileSelected(coreui::Project &frame,
                                         std::string file_path) {
  if (const auto not_json_extension =
          !AsLowerCase(file_path).ends_with(".json")) {
    file_path += ".json";
  }

  frame.SaveToFile(std::move(file_path));
}
}  // namespace esc::draw