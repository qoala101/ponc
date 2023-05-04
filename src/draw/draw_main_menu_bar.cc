#include "draw_main_menu_bar.h"

#include <imgui.h>

#include <filesystem>
#include <functional>
#include <optional>
#include <variant>

#include "core_project.h"
#include "core_settings.h"
#include "coreui_diagram.h"
#include "coreui_project.h"
#include "draw_i_view.h"
#include "draw_native_facade.h"
#include "draw_open_file_dialog.h"
#include "draw_question_dialog.h"
#include "draw_save_as_file_dialog.h"
#include "flow_algorithms.h"
#include "imgui_node_editor.h"

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
           .ok_label = "New Project"}} {}

///
void MainMenuBar::Draw(coreui::Project &project) {
  if (ImGui::BeginMainMenuBar()) {
    DrawFileMenu(project);
    DrawViewMenu();
    ImGui::Separator();

    ImGui::MenuItem("Color Flow", nullptr,
                    &project.GetProject().GetSettings().color_flow);
    ImGui::Separator();

    if (ImGui::MenuItem("Zoom To Content")) {
      ne::NavigateToContent();
    }

    ImGui::EndMainMenuBar();
  }

  DrawDialogs(project);
  DrawViews(project);
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
    ImGui::Separator();

    DrawViewMenuItem(calculator_view_);
    ImGui::Separator();

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
}

///
void MainMenuBar::DrawViews(coreui::Project &project) {
  const auto &diagram = project.GetDiagram();
  const auto selected_nodes = NativeFacade::GetSelectedNodes();
  const auto selected_node =
      (selected_nodes.size() == 1)
          ? &coreui::Diagram::FindNode(diagram, selected_nodes.front())
          : std::optional<const coreui::Node *>{};

  node_view_.Draw(selected_node, diagram.GetFamilyGroups());
  nodes_view_.Draw(diagram);
  diagrams_view_.Draw(project);
  flow_tree_view_.Draw(diagram.GetFlowTrees());

  auto &core_project = project.GetProject();

  calculator_view_.Draw(project.GetCalculator(), core_project);
  settings_view_.Draw(core_project.GetSettings());
}
}  // namespace vh::ponc::draw