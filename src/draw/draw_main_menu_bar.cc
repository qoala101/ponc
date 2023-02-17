/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#include "draw_main_menu_bar.h"

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
#include "imgui.h"

namespace esc::draw {
namespace {
// ---
void DrawFileMenu(State& state) {
  if (ImGui::BeginMenu("File")) {
    const auto menu_scope = cpp::Scope{[]() { ImGui::EndMenu(); }};

    if (ImGui::MenuItem("Open...", nullptr)) {
      state.draw_state->open_file_dialog.Open();
    }

    if (ImGui::MenuItem("Save As...")) {
      state.draw_state->save_as_file_dialog.Open();
    }

    if (ImGui::MenuItem("Reset")) {
      state.event_queue->PostEvent(event::ResetDiagram{});
    }
  }

  DrawOpenFileDialog(state);
  DrawSaveAsFileDialog(state);
}

// ---
void DrawViewsMenu(State& state) {
  if (ImGui::BeginMenu("Views")) {
    const auto menu_scope = cpp::Scope{[]() { ImGui::EndMenu(); }};

    ImGui::MenuItem("Objects", nullptr,
                    state.draw_state->families_view_visible);
    ImGui::MenuItem("Flow Tree", nullptr,
                    state.draw_state->families_view_visible);
    ImGui::MenuItem("Groups", nullptr, state.draw_state->families_view_visible);
    ImGui::MenuItem("Group Settings", nullptr,
                    state.draw_state->families_view_visible);
    ImGui::MenuItem("Settings", nullptr,
                    state.draw_state->families_view_visible);
  }

  DrawFamiliesView(state);
  DrawFlowTreeView(state);
  DrawGroupsView(state);
  DrawGroupSettingsView(state);
  DrawSettingsView(state);
}
}  // namespace

// ---
void DrawMainMenuBar(State& state) {
  if (ImGui::BeginMainMenuBar()) {
    const auto main_menu_bar_scope =
        cpp::Scope{[]() { ImGui::EndMainMenuBar(); }};

    DrawFileMenu(state);
    DrawViewsMenu(state);

    if (ImGui::MenuItem("Zoom to Content")) {
      ne::NavigateToContent();
    }

    if (ImGui::MenuItem("Show Flow")) {
      for (const auto& link : state.core_state->diagram_.GetLinks()) {
        ne::Flow(link.id);
      }
    }
  }
}
}  // namespace esc::draw