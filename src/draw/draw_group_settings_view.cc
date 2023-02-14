#include "draw_group_settings_view.h"

#include <unordered_set>

#include "core_group.h"
#include "core_i_node.h"
#include "cpp_scope.h"
#include "draw_groups_view.h"
#include "draw_i_family_drawer.h"
#include "draw_i_node_drawer.h"
#include "imgui.h"
#include "imgui_node_editor.h"

namespace esc::draw {
void GroupSettingsView::Draw(State& state) {
  if (!IsVisible()) {
    return;
  }

  {
    const auto window_scope = cpp::Scope{[]() { ImGui::End(); }};

    if (ImGui::Begin("Group Settings", &GetVisible())) {
      auto& groups = state.app_.GetDiagram().GetGroups();

      // Left
      static int selected = 0;
      {
        ImGui::BeginChild("left pane", ImVec2(200, 0), true);

        auto i = 0;

        for (auto& group : groups) {
          if (ImGui::Selectable(group.name_.c_str(), selected == i)) {
            selected = i;
          }

          ++i;
        }
        ImGui::EndChild();
      }
      ImGui::SameLine();

      // Right
      {
        ImGui::BeginGroup();                                       // 1 line below us
        auto& group = groups[selected];

        ImGui::Checkbox("Fill##asdas", &group.fill_background_);
        ImGui::Checkbox("Unite##hhhh", &group.unite_);

        static bool alpha_preview = true;
        static bool alpha_half_preview = false;
        static bool drag_and_drop = true;
        static bool options_menu = true;
        static bool hdr = false;
        ImGuiColorEditFlags misc_flags =
            (hdr ? ImGuiColorEditFlags_HDR : 0) |
            (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) |
            (alpha_half_preview
                 ? ImGuiColorEditFlags_AlphaPreviewHalf
                 : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) |
            (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

        ImGui::ColorEdit3("Color##3asdas", group.color_.data(),
                          ImGuiColorEditFlags_NoInputs  | misc_flags);

        ImGui::EndGroup();
      }
    }
  }
}
}  // namespace esc::draw