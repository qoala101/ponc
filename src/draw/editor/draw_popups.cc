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
      state.draw_state->background_popup.SetPosition(popup_position);
      state.draw_state->background_popup.Show();
      return;
    }

    auto selected_node_id = ne::NodeId{};

    if (ne::ShowNodeContextMenu(&selected_node_id)) {
      state.draw_state->node_popup.SetNodeId(selected_node_id);
      state.draw_state->node_popup.Show();
      return;
    }

    auto selected_link_id = ne::LinkId{};

    if (ne::ShowLinkContextMenu(&selected_link_id)) {
      state.draw_state->link_popup.SetLinkId(selected_link_id);
      state.draw_state->link_popup.Show();
      return;
    }
  }
}

void DrawPopupContents(State& state) {
  const auto suspend_scope =
      cpp::Scope{[]() { ne::Suspend(); }, []() { ne::Resume(); }};

  state.draw_state->background_popup.Draw(state);
  state.draw_state->node_popup.Draw(state);
  state.draw_state->link_popup.Draw(state);
}
}  // namespace

void DrawPopups(State& state) {
  DrawOpenPopupProcess(state);
  DrawPopupContents(state);
}
}  // namespace esc::draw