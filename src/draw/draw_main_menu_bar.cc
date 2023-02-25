/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#include "draw_main_menu_bar.h"

#include <imgui.h>

#include "app_state.h"
#include "core_project.h"
#include "cpp_scope.h"
#include "draw_open_file_dialog.h"
#include "draw_save_as_file_dialog.h"
#include "draw_settings_view.h"
#include "draw_widgets.h"
#include "frame_node.h"

namespace esc::draw {
namespace {
void DrawViewMenuItem(auto& view) {
  const auto visible = view.IsVisible();

  if (ImGui::MenuItem(view.GetLabel().c_str(), nullptr, visible)) {
    view.SetVisible(!visible);
  }
}
}  // namespace

void MainMenuBar::Draw(const AppState& app_state) {
  if (ImGui::BeginMainMenuBar()) {
    const auto main_menu_bar_scope =
        cpp::Scope{[]() { ImGui::EndMainMenuBar(); }};

    DrawFileMenu(app_state);
    DrawViewsMenu(app_state);

    if (ImGui::MenuItem("Zoom to Content")) {
      ne::NavigateToContent();
    }

    if (ImGui::MenuItem("Show Flow")) {
      for (const auto& link :
           app_state.frame.GetProject().GetDiagram().GetLinks()) {
        ne::Flow(link.id);
      }
    }
  }
}

void MainMenuBar::DrawFileMenu(const AppState& app_state) {
  if (ImGui::BeginMenu("File")) {
    const auto menu_scope = cpp::Scope{[]() { ImGui::EndMenu(); }};

    if (ImGui::MenuItem("Open...", nullptr)) {
      open_file_dialog.Show();
    }

    if (ImGui::MenuItem("Save As...")) {
      save_as_file_dialog.Show();
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Reset")) {
      // app_state.ResetDiagram{});
    }
  }

  open_file_dialog.Draw(app_state);
  save_as_file_dialog.Draw(app_state);
}

void MainMenuBar::DrawViewsMenu(const AppState& app_state) {
  if (ImGui::BeginMenu("Views")) {
    const auto menu_scope = cpp::Scope{[]() { ImGui::EndMenu(); }};

    // DrawViewMenuItem(state.draw_state->families_view_);
    // DrawViewMenuItem(state.draw_state->flow_tree_view_);
    // DrawViewMenuItem(state.draw_state->groups_view_);
    // DrawViewMenuItem(state.draw_state->group_settings_view_);
    DrawViewMenuItem(settings_view);
  }

  // state.draw_state->families_view_.Draw(state);
  // state.draw_state->flow_tree_view_.Draw(state);
  // state.draw_state->groups_view_.Draw(state);
  // state.draw_state->group_settings_view_.Draw(state);
  settings_view.Draw(app_state);
}
}  // namespace esc::draw