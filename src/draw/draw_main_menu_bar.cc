/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#include "draw_main_menu_bar.h"

#include <imgui.h>

#include "app_events.h"
#include "app_state.h"
#include "cpp_scope.h"
#include "draw_families_view.h"
#include "draw_flow_tree_view.h"
#include "draw_group_settings_view.h"
#include "draw_groups_view.h"
#include "draw_open_file_dialog.h"
#include "draw_save_as_file_dialog.h"
#include "draw_settings_view.h"
#include "draw_widgets.h"

namespace esc::draw {
namespace {
// ---
void DrawFileMenu(const AppState& app_state) {
  if (ImGui::BeginMenu("File")) {
    const auto menu_scope = cpp::Scope{[]() { ImGui::EndMenu(); }};

    if (ImGui::MenuItem("Open...", nullptr)) {
      app_state.widgets->open_file_dialog.Show();
    }

    if (ImGui::MenuItem("Save As...")) {
      app_state.widgets->save_as_file_dialog.Show();
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Reset")) {
      // app_state.event_queue->PostEvent(event::ResetDiagram{});
    }
  }

  app_state.widgets->open_file_dialog.Draw(app_state);
  app_state.widgets->save_as_file_dialog.Draw(app_state);
}

// ---
void DrawViewMenuItem(auto& view) {
  const auto visible = view.IsVisible();

  if (ImGui::MenuItem(view.GetLabel().c_str(), nullptr, visible)) {
    view.SetVisible(!visible);
  }
}

// ---
void DrawViewsMenu(const AppState& app_state) {
  if (ImGui::BeginMenu("Views")) {
    const auto menu_scope = cpp::Scope{[]() { ImGui::EndMenu(); }};

    // DrawViewMenuItem(state.draw_state->families_view_);
    // DrawViewMenuItem(state.draw_state->flow_tree_view_);
    // DrawViewMenuItem(state.draw_state->groups_view_);
    // DrawViewMenuItem(state.draw_state->group_settings_view_);
    // DrawViewMenuItem(state.draw_state->settings_view_);
  }

  // state.draw_state->families_view_.Draw(state);
  // state.draw_state->flow_tree_view_.Draw(state);
  // state.draw_state->groups_view_.Draw(state);
  // state.draw_state->group_settings_view_.Draw(state);
  // state.draw_state->settings_view_.Draw(state);
}
}  // namespace

// ---
void DrawMainMenuBar(const AppState& app_state) {
  if (ImGui::BeginMainMenuBar()) {
    const auto main_menu_bar_scope =
        cpp::Scope{[]() { ImGui::EndMainMenuBar(); }};

    DrawFileMenu(app_state);
    DrawViewsMenu(app_state);

    if (ImGui::MenuItem("Zoom to Content")) {
      ne::NavigateToContent();
    }

    if (ImGui::MenuItem("Show Flow")) {
      for (const auto& link : app_state.project->GetDiagram().GetLinks()) {
        ne::Flow(link.id);
      }
    }
  }
}
}  // namespace esc::draw