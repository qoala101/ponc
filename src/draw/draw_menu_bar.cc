#include "draw_menu_bar.h"

#include "cpp_scope.h"
#include "draw_open_file_dialog.h"
#include "draw_save_as_file_dialog.h"
#include "esc_app_state.h"
#include "imgui.h"

namespace esc::draw {
MenuBar::MenuBar(std::shared_ptr<AppState> app_state)
    : app_state_{std::move(app_state)},
      open_file_dialog_{app_state_},
      save_as_file_dialog_{app_state_} {}

void MenuBar::Draw() {
  if (ImGui::BeginMainMenuBar()) {
    const auto menu_bar_scope = cpp::Scope{[]() { ImGui::EndMainMenuBar(); }};

    if (ImGui::BeginMenu("File")) {
      const auto menu_scope = cpp::Scope{[]() { ImGui::EndMenu(); }};

      if (ImGui::MenuItem("Open...")) {
        open_file_dialog_.SetVisible(true);
      }

      if (ImGui::MenuItem("Save As...")) {
        save_as_file_dialog_.SetVisible(true);
      }

      if (ImGui::MenuItem("Reset")) {
        app_state_->ResetDiagram();
      }
    }

    if (ImGui::BeginMenu("Views")) {
      const auto menu_scope = cpp::Scope{[]() { ImGui::EndMenu(); }};

      if (ImGui::MenuItem("Nodes", nullptr, nodes_view_.IsVisible())) {
        nodes_view_.SetVisible(!nodes_view_.IsVisible());
      }

      if (ImGui::MenuItem("Node Factories", nullptr,
                          node_factories_view_.IsVisible())) {
        node_factories_view_.SetVisible(!node_factories_view_.IsVisible());
      }
    }

    if (ImGui::MenuItem("Zoom to Content")) {
      ne::NavigateToContent();
    }

    if (ImGui::MenuItem("Show Flow")) {
      for (const auto& link : app_state_->app_.GetDiagram()->GetLinks()) {
        ne::Flow(link.id);
      }
    }
  }

  open_file_dialog_.Draw();
  save_as_file_dialog_.Draw();
  nodes_view_.Draw();
  node_factories_view_.Draw();
}
}  // namespace esc::draw