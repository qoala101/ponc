#include "draw_group_settings_view.h"

#include <unordered_set>

#include "core_group.h"
#include "core_i_node.h"
#include "coreui_i_family_drawer.h"
#include "coreui_i_node_drawer.h"
#include "cpp_scope.h"
#include "draw_groups_view.h"
#include "imgui.h"
#include "imgui_node_editor.h"

namespace esc::draw {
void DrawGroupSettingsView(State& state) {
  if (!state.draw_state->group_settings_view_visible) {
    return;
  }

  {
    const auto window_scope = cpp::Scope{[]() { ImGui::End(); }};

    if (ImGui::Begin("Group Settings",
                     &state.draw_state->group_settings_view_visible)) {
      auto& groups = state.core_state->diagram_.GetGroups();

      {
        ImGui::BeginChild("left pane", ImVec2(200, 0), true);

        auto group_index = 0;

        for (auto& group : groups) {
          if (ImGui::Selectable(
                  group.name_.c_str(),
                  state.draw_state->group_setings_view_selected_group_index ==
                      group_index)) {
            state.draw_state->group_setings_view_selected_group_index =
                group_index;
          }

          ++group_index;
        }

        ImGui::EndChild();
      }
      ImGui::SameLine();

      // Right
      {
        ImGui::BeginGroup();  // 1 line below us

        if (!groups.empty()) {
          auto& group =
              groups[state.draw_state->group_setings_view_selected_group_index];

          ImGui::Checkbox("Fill##asdas", &group.fill_background_);
          ImGui::Checkbox("Unite##hhhh", &group.unite_);
          ImGui::ColorEdit3("Color##3asdas", group.color_.data(),
                            ImGuiColorEditFlags_NoInputs);
        }

        ImGui::EndGroup();
      }
    }
  }
}
}  // namespace esc::draw