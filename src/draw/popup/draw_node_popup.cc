#include "draw_node_popup.h"

#include <iostream>

#include "cpp_scope.h"
#include "draw_i_node_drawer.h"
#include "imgui.h"

namespace esc::draw {
void DrawNodePopup(State& state) {
  if (ImGui::BeginPopup("Node")) {
    const auto popup_scope = cpp::Scope{[]() { ImGui::EndPopup(); }};

    ImGui::TextUnformatted("Node");
    ImGui::Separator();

    if (ImGui::MenuItem("Delete With Links")) {
      state.PostEvent([](auto& state) {
        State::EraseNodeAndConnectedLinks(state, state.DRAW_.popup_node_id);
      });
    }

    if (ImGui::MenuItem("Delete")) {
      state.PostEvent([](auto& state) {
        State::ReplaceWithFreePins(state, state.DRAW_.popup_node_id);
      });
    }

    if (ImGui::BeginMenu("Group")) {
      const auto menu_scope = cpp::Scope{[]() { ImGui::EndMenu(); }};

      ImGui::InputTextWithHint("", "Enter group name...",
                               state.DRAW_.popup_group_name.data(),
                               state.DRAW_.popup_group_name.size());
      ImGui::SameLine();

      if (ImGui::SmallButton("Add")) {
        state.PostEvent([](auto& state) {
          State::MakeGroupFromSelectedNodes(
              state, std::string{state.DRAW_.popup_group_name.begin(),
                                 state.DRAW_.popup_group_name.end()});
        });

        // ImGui::CloseCurrentPopup();
      }
    }
  }
}
}  // namespace esc::draw