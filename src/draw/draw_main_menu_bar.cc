/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#include "draw_main_menu_bar.h"

#include "cpp_scope.h"
#include "esc_state.h"
#include "imgui.h"

namespace esc::draw {
namespace {
// ---
void DrawFileMenu(State& state) {
  if (ImGui::BeginMenu("File")) {
    const auto menu_scope = cpp::Scope{[]() { ImGui::EndMenu(); }};

    if (ImGui::MenuItem("Open...", nullptr)) {
      state.DRAW_.open_file_dialog.Open();
    }

    if (ImGui::MenuItem("Save As...")) {
      state.DRAW_.save_as_file_dialog.Open();
    }

    if (ImGui::MenuItem("Reset")) {
      state.PostEvent([](auto& state) { State::ResetDiagram(state); });
    }
  }
}

// ---
void DrawViewsMenu(State& state) {
  if (ImGui::BeginMenu("Views")) {
    const auto menu_scope = cpp::Scope{[]() { ImGui::EndMenu(); }};

    ImGui::MenuItem("Objects", nullptr, state.DRAW_.families_view_visible);
    ImGui::MenuItem("Flow Tree", nullptr, state.DRAW_.families_view_visible);
    ImGui::MenuItem("Groups", nullptr, state.DRAW_.families_view_visible);
    ImGui::MenuItem("Group Settings", nullptr,
                    state.DRAW_.families_view_visible);
    ImGui::MenuItem("Settings", nullptr, state.DRAW_.families_view_visible);
  }
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
      for (const auto& link : state.diagram_.GetLinks()) {
        ne::Flow(link.id);
      }
    }
  }
}
}  // namespace esc::draw