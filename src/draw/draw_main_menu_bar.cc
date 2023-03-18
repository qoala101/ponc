#include "draw_main_menu_bar.h"

#include <imgui.h>

#include <functional>
#include <variant>

#include "core_settings.h"
#include "coreui_project.h"
#include "draw_open_file_dialog.h"
#include "draw_save_as_file_dialog.h"

namespace esc::draw {
namespace {
///
auto AsLowerCase(std::string text) {
  for (auto &character : text) {
    character = static_cast<char>(std::tolower(character));
  }

  return text;
}
}  // namespace

///
void MainMenuBar::Draw(coreui::Project &project) {
  auto &settings = project.GetProject().GetSettings();

  if (ImGui::BeginMainMenuBar()) {
    DrawFileMenu(project);
    DrawViewMenu();

    if (ImGui::MenuItem("Zoom to Content")) {
      ne::NavigateToContent();
    }

    ImGui::MenuItem("Color Flow", nullptr, &settings.color_flow);
    ImGui::EndMainMenuBar();
  }

  DrawDialogs(project);
  DrawViews(project.GetDiagram(), settings);
}

///
void MainMenuBar::DrawFileMenu(coreui::Project &project) {
  if (ImGui::BeginMenu("File")) {
    if (ImGui::MenuItem("New...")) {
      new_project_dialog_.Open();
    }

    if (ImGui::MenuItem("Open...")) {
      open_file_dialog_.Open();
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Save", nullptr, false, project.CanSave())) {
      project.Save();
    }

    if (ImGui::MenuItem("Save As...")) {
      save_as_file_dialog_.Open();
    }

    ImGui::EndMenu();
  }
}

///
void MainMenuBar::DrawViewMenu() {
  if (ImGui::BeginMenu("View")) {
    if (ImGui::MenuItem(nodes_view_.GetLabel().c_str(), nullptr,
                        nodes_view_.IsOpened())) {
      nodes_view_.Toggle();
    }

    if (ImGui::MenuItem("Flow Tree")) {
    }

    ImGui::Separator();

    if (ImGui::MenuItem(settings_view_.GetLabel().c_str(), nullptr,
                        settings_view_.IsOpened())) {
      settings_view_.Toggle();
    }

    ImGui::EndMenu();
  }
}

///
void MainMenuBar::DrawDialogs(coreui::Project &project) {
  new_project_dialog_.Draw({.accepted = [&project]() { project.Reset(); }});

  open_file_dialog_.Draw({.file_selected = [&project](auto file_path) {
    project.OpenFromFile(std::move(file_path));
  }});

  save_as_file_dialog_.Draw({.file_selected = [&project](auto file_path) {
    if (const auto not_json_extension =
            !AsLowerCase(file_path).ends_with(".json")) {
      file_path += ".json";
    }

    project.SaveToFile(std::move(file_path));
  }});
}

///
void MainMenuBar::DrawViews(const coreui::Diagram &diagram,
                            core::Settings &settings) {
  nodes_view_.Draw(diagram);
  settings_view_.Draw(settings);
}
}  // namespace esc::draw