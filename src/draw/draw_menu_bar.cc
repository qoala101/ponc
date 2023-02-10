#include "draw_menu_bar.h"

#include "imgui.h"

namespace esc::draw {
void MenuBar::Draw() {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Open...")) {
        open_file_dialog_.SetVisible(true);
      }

      if (ImGui::MenuItem("Save As...")) {
        save_as_file_dialog_.SetVisible(true);
      }

      if (ImGui::MenuItem("Clear")) {
        // app_->GetNodesAndLinks().DeleteAll();
      }

      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Views")) {
      if (ImGui::MenuItem("Nodes", nullptr, nodes_view_.IsVisible())) {
        nodes_view_.SetVisible(!nodes_view_.IsVisible());
      }

      if (ImGui::MenuItem("Node Factories", nullptr,
                          node_factories_view_.IsVisible())) {
        node_factories_view_.SetVisible(!node_factories_view_.IsVisible());
      }

      ImGui::EndMenu();
    }

    if (ImGui::MenuItem("Zoom to Content")) {
      // ne::NavigateToContent();
    }

    if (ImGui::MenuItem("Show Flow")) {
      // app_->ShowFlow();
    }

    ImGui::EndMainMenuBar();
  }

  open_file_dialog_.Draw();
  save_as_file_dialog_.Draw();
  nodes_view_.Draw();
  node_factories_view_.Draw();
}
}  // namespace esc::draw