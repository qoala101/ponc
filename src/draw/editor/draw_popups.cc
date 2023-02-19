/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#include "draw_popups.h"

#include "app_state.h"
#include "cpp_scope.h"
#include "draw_background_popup.h"
#include "draw_link_popup.h"
#include "draw_node_popup.h"
#include "draw_widgets.h"

namespace esc::draw {
namespace {
// ---
void DrawShowPopupProcess(const AppState &app_state) {
  const auto popup_position = ImGui::GetMousePos();

  {
    const auto suspend_scope =
        cpp::Scope{[]() { ne::Suspend(); }, []() { ne::Resume(); }};

    if (ne::ShowBackgroundContextMenu()) {
      app_state.widgets->background_popup.SetPosition(popup_position);
      app_state.widgets->background_popup.Show();
      return;
    }

    auto popup_node_id = ne::NodeId{};

    if (ne::ShowNodeContextMenu(&popup_node_id)) {
      app_state.widgets->node_popup.SetNodeId(popup_node_id);
      app_state.widgets->node_popup.Show();
      return;
    }

    auto popup_link_id = ne::LinkId{};

    if (ne::ShowLinkContextMenu(&popup_link_id)) {
      app_state.widgets->link_popup.SetLinkId(popup_link_id);
      app_state.widgets->link_popup.Show();
      return;
    }
  }
}

// ---
void DrawPopupContents(const AppState &app_state) {
  const auto suspend_scope =
      cpp::Scope{[]() { ne::Suspend(); }, []() { ne::Resume(); }};

  app_state.widgets->background_popup.Draw(app_state);
  app_state.widgets->node_popup.Draw(app_state);
  app_state.widgets->link_popup.Draw(app_state);
}
}  // namespace

// ---
void DrawPopups(const AppState &app_state) {
  DrawShowPopupProcess(app_state);
  DrawPopupContents(app_state);
}
}  // namespace esc::draw