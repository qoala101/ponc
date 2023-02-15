#include "draw_popups.h"

#include <iostream>

#include "cpp_scope.h"
#include "draw_background_popup.h"
#include "draw_link_popup.h"
#include "draw_node_popup.h"

namespace esc::draw {
// ---
void Popups::Draw(State& state) {
  UpdateCurrentPopup(state);
  DrawCurrentPopup(state);
}

// ---
void Popups::UpdateCurrentPopup(State& state) {
  const auto popup_position = ImGui::GetMousePos();

  {
    const auto suspend_scope =
        cpp::Scope{[]() { ne::Suspend(); }, []() { ne::Resume(); }};

    if (ne::ShowBackgroundContextMenu()) {
      current_popup_.emplace(std::make_unique<BackgroundPopup>(popup_position))
          ->Open(state);

      return;
    }

    auto selected_node_id = ne::NodeId{};

    if (ne::ShowNodeContextMenu(&selected_node_id)) {
      state.drawing_.popup_node_ = selected_node_id;

      const auto& selected_node =
          state.app_.GetDiagram().FindNodePTR(selected_node_id);

      current_popup_.emplace(std::make_unique<NodePopup>(selected_node))
          ->Open(state);

      return;
    }

    auto selected_link_id = ne::LinkId{};

    if (ne::ShowLinkContextMenu(&selected_link_id)) {
      const auto& selected_link =
          state.app_.GetDiagram().FindLink(selected_link_id);

      current_popup_.emplace(std::make_unique<LinkPopup>(selected_link))
          ->Open(state);
    }
  }
}

// ---
void Popups::DrawCurrentPopup(State& state) {
  const auto suspend_scope =
      cpp::Scope{[]() { ne::Suspend(); }, []() { ne::Resume(); }};

  if (current_popup_.has_value()) {
    (*current_popup_)->Draw(state);

    if (!(*current_popup_)->IsVisible()) {
      current_popup_.reset();
    }
  }
}
}  // namespace esc::draw