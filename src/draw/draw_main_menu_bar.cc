/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_main_menu_bar.h"

#include <imgui.h>
#include <imgui_node_editor.h>

#include <cctype>
#include <filesystem>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "core_project.h"
#include "core_settings.h"
#include "coreui_diagram.h"
#include "coreui_native_facade.h"
#include "coreui_node.h"
#include "coreui_project.h"
#include "draw_about_dialog.h"
#include "draw_i_view.h"
#include "draw_open_file_dialog.h"
#include "draw_question_dialog.h"
#include "draw_save_as_file_dialog.h"

namespace vh::ponc::draw {
namespace {
///
auto AsLowerCase(std::string text) {
  for (auto &character : text) {
    character = static_cast<char>(std::tolower(character));
  }

  return text;
}

///
void DrawViewMenuItem(IView &view) {
  if (ImGui::MenuItem(view.GetLabel().c_str(), nullptr, view.IsOpened())) {
    view.Toggle();
  }
}
}  // namespace

///
MainMenuBar::MainMenuBar()
    : new_project_dialog_{
          {.title = "New Project",
           .question = "Current project would not be saved.\nProceed?",
           .accept = "New Project"}} {}

///
void MainMenuBar::Draw(coreui::Project &project) {
  if (ImGui::BeginMainMenuBar()) {
    DrawFileMenu(project);
    DrawViewMenu();

    ImGui::MenuItem("Color Flow", nullptr,
                    &project.GetProject().GetSettings().color_flow);

    if (ImGui::MenuItem("Zoom To Content")) {
      ne::NavigateToContent();
    }

    if (ImGui::MenuItem("About")) {
      about_dialog_.Open();
    }

    ImGui::EndMainMenuBar();
  }

  DrawDialogs(project);
  DrawViews(project);
}

///
auto MainMenuBar::GetLogView() const -> const LogView & { return log_view_; }

///
void MainMenuBar::SaveState(Globals &globals) const {
  globals.Set("node_view_opened", nodes_view_.IsOpened());
  globals.Set("nodes_view_opened", nodes_view_.IsOpened());
  globals.Set("diagrams_view_opened", diagrams_view_.IsOpened());
  globals.Set("flow_tree_view_opened", flow_tree_view_.IsOpened());
  globals.Set("connections_view_opened", connections_view_.IsOpened());
  globals.Set("calculator_view_opened", calculator_view_.IsOpened());
  globals.Set("log_view_opened", log_view_.IsOpened());
  globals.Set("settings_view_opened", settings_view_.IsOpened());
}

///
void MainMenuBar::RestoreState(const Globals &globals) {
  node_view_.SetOpened(globals.GetOr("node_view_opened", false));
  nodes_view_.SetOpened(globals.GetOr("nodes_view_opened", false));
  diagrams_view_.SetOpened(globals.GetOr("diagrams_view_opened", false));
  flow_tree_view_.SetOpened(globals.GetOr("flow_tree_view_opened", false));
  connections_view_.SetOpened(globals.GetOr("connections_view_opened", false));
  calculator_view_.SetOpened(globals.GetOr("calculator_view_opened", false));
  log_view_.SetOpened(globals.GetOr("log_view_opened", false));
  settings_view_.SetOpened(globals.GetOr("settings_view_opened", false));
}

///
void MainMenuBar::DrawFileMenu(coreui::Project &project) {
  if (ImGui::BeginMenu("File")) {
    if (ImGui::MenuItem("New...")) {
      if (core::Project::IsEmpty(project.GetProject())) {
        project.Reset();
      } else {
        new_project_dialog_.Open();
      }
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
    DrawViewMenuItem(node_view_);
    DrawViewMenuItem(nodes_view_);
    DrawViewMenuItem(diagrams_view_);
    DrawViewMenuItem(flow_tree_view_);
    DrawViewMenuItem(connections_view_);
    ImGui::Separator();

    DrawViewMenuItem(calculator_view_);
    ImGui::Separator();

    DrawViewMenuItem(log_view_);
    DrawViewMenuItem(settings_view_);
    ImGui::EndMenu();
  }
}

///
void MainMenuBar::DrawDialogs(coreui::Project &project) {
  new_project_dialog_.Draw({.accepted = [&project]() { project.Reset(); }});

  const auto requires_confirmation =
      !core::Project::IsEmpty(project.GetProject());

  open_file_dialog_.Draw(
      {.file_selected =
           [&project](auto file_path) {
             project.OpenFromFile(std::move(file_path)).Then(([]() {
               ne::NavigateToContent();
             }));
           }},
      requires_confirmation);

  save_as_file_dialog_.Draw({.file_selected = [&project](auto file_path) {
    if (AsLowerCase(file_path.extension().string()) != ".json") {
      file_path.replace_extension(".json");
    }

    project.SaveToFile(std::move(file_path));
  }});

  about_dialog_.Draw();
}

///
void MainMenuBar::DrawViews(coreui::Project &project) {
  const auto &diagram = project.GetDiagram();
  const auto selected_nodes = coreui::NativeFacade::GetSelectedNodes();
  const auto selected_node =
      (selected_nodes.size() == 1)
          ? &coreui::Diagram::FindNode(diagram, selected_nodes.front())
          : std::optional<const coreui::Node *>{};

  node_view_.Draw(selected_node, diagram.GetFamilyGroups());
  nodes_view_.Draw(diagram);
  connections_view_.Draw(project);
  diagrams_view_.Draw(project);
  flow_tree_view_.Draw(diagram.GetNodeTrees());

  auto &core_project = project.GetProject();

  calculator_view_.Draw(project.GetCalculator(), core_project);
  log_view_.Draw(project.GetLog());
  settings_view_.Draw(core_project.GetSettings());
}
}  // namespace vh::ponc::draw