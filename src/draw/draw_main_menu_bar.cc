#include "draw_main_menu_bar.h"

#include <imgui.h>

#include <functional>
#include <optional>
#include <variant>

#include "core_settings.h"
#include "coreui_diagram.h"
#include "coreui_project.h"
#include "draw_i_view.h"
#include "draw_native_facade.h"
#include "draw_open_file_dialog.h"
#include "draw_save_as_file_dialog.h"
#include "imgui_node_editor.h"

namespace esc::draw {
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
    DrawViewMenuItem(node_view_);
    DrawViewMenuItem(nodes_view_);
    DrawViewMenuItem(flow_tree_view_);

    ImGui::Separator();
    DrawViewMenuItem(settings_view_);

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
  const auto selected_nodes = NativeFacade::GetSelectedNodes();
  const auto selected_node =
      (selected_nodes.size() == 1)
          ? &coreui::Diagram::FindNode(diagram, selected_nodes.front())
          : std::optional<const coreui::Node *>{};

  node_view_.Draw(selected_node, diagram.GetFamilyGroups());
  nodes_view_.Draw(diagram);
  flow_tree_view_.Draw(diagram.GetFlowTree());
  settings_view_.Draw(settings);
}
}  // namespace esc::draw