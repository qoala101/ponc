#include "draw_popups.h"

#include <iostream>

#include "cpp_scope.h"
#include "draw_background_popup.h"
#include "draw_link_popup.h"
#include "draw_node_popup.h"

namespace esc::draw {
namespace {
void DrawOpenPopupProcess(State& state) {
  const auto popup_position = ImGui::GetMousePos();

  {
    const auto suspend_scope =
        cpp::Scope{[]() { ne::Suspend(); }, []() { ne::Resume(); }};

    if (ne::ShowBackgroundContextMenu()) {
      state.DRAW_.popup_position = popup_position;
      ImGui::OpenPopup("Create New Node",
                       ImGuiPopupFlags_NoOpenOverExistingPopup);
      return;
    }

    auto selected_node_id = ne::NodeId{};

    if (ne::ShowNodeContextMenu(&selected_node_id)) {
      state.DRAW_.popup_node_id = selected_node_id;
      ImGui::OpenPopup("Node", ImGuiPopupFlags_NoOpenOverExistingPopup);
      return;
    }

    auto selected_link_id = ne::LinkId{};

    if (ne::ShowLinkContextMenu(&selected_link_id)) {
      state.DRAW_.popup_link_id = selected_link_id;
      ImGui::OpenPopup("Link", ImGuiPopupFlags_NoOpenOverExistingPopup);
      return;
    }
  }
}

void DrawPopupContents(State& state) {
  const auto suspend_scope =
      cpp::Scope{[]() { ne::Suspend(); }, []() { ne::Resume(); }};

  DrawBackgroundPopup(state);
  DrawNodePopup(state);
  DrawLinkPopup(state);
}
}  // namespace

void DrawPopups(State& state) {
  DrawOpenPopupProcess(state);
  DrawPopupContents(state);
}
}  // namespace esc::draw