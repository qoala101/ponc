#include "draw_menu_bar.h"

#include "cpp_scope.h"
#include "draw_open_file_dialog.h"
#include "draw_save_as_file_dialog.h"
#include "esc_state.h"
#include "imgui.h"

namespace esc::draw {
void MenuBar::Draw(State& state) {
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
        state.ResetDiagram();
      }
    }

    if (ImGui::MenuItem("Families", nullptr, families_view_.IsVisible())) {
      families_view_.SetVisible(!families_view_.IsVisible());
    }

    if (ImGui::MenuItem("Zoom to Content")) {
      ne::NavigateToContent();
    }

    if (ImGui::MenuItem("Show Flow")) {
      for (const auto& link : state.app_.GetDiagram().GetLinks()) {
        ne::Flow(link.id);
      }
    }
  }

  open_file_dialog_.Draw(state);
  save_as_file_dialog_.Draw(state);
  families_view_.Draw(state);
}
}  // namespace esc::draw