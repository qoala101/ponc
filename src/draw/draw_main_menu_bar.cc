#include "draw_main_menu_bar.h"

#include <imgui.h>

#include <functional>
#include <variant>

#include "core_link.h"
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

///
void ShowFlow(const std::vector<core::Link> &links) {
  for (const auto &link : links) {
    ne::Flow(link.id);
  }
}
}  // namespace

///
void MainMenuBar::Draw(coreui::Project &project) {
  if (ImGui::BeginMainMenuBar()) {
    DrawFileMenu(project);

    if (ImGui::MenuItem("Zoom to Content")) {
      ne::NavigateToContent();
    }

    if (ImGui::MenuItem("Show Flow")) {
      ShowFlow(project.GetDiagram().GetDiagram().GetLinks());
    }

    if (ImGui::MenuItem("Color Flow", nullptr,
                        &project.GetProject().GetSettings().color_flow)) {
    }

    ImGui::EndMainMenuBar();
  }

  DrawDialogs(project);
}

///
void MainMenuBar::DrawFileMenu(coreui::Project &project) {
  if (ImGui::BeginMenu("File")) {
    if (ImGui::MenuItem("Open...", nullptr)) {
      open_file_dialog_.Show();
    }

    if (ImGui::MenuItem("Save As...")) {
      save_as_file_dialog_.Show();
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Reset")) {
      project.Reset();
    }

    ImGui::EndMenu();
  }
}

///
void MainMenuBar::DrawDialogs(coreui::Project &project) {
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
}  // namespace esc::draw