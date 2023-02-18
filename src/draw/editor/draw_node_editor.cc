#include "draw_node_editor.h"

#include "app_state.h"
#include "cpp_scope.h"
#include "draw_delete_items_process.h"
#include "draw_groups.h"
#include "draw_link_being_repinned.h"
#include "draw_link_connection_process.h"
#include "draw_links.h"
#include "draw_nodes.h"
#include "draw_popups.h"

namespace esc::draw {
void DrawNodeEditor(State &state) {
  const auto node_editor_scope =
      cpp::Scope{[]() { ne::Begin("Node editor"); }, []() { ne::End(); }};

  state.draw_state->link_being_repinned.Draw(state);
  draw::DrawGroups(state);
  state.draw_state->links.Draw(state);
  state.draw_state->nodes.Draw(state);
  state.draw_state->link_connection_process.Draw(state);
  draw::DrawDeleteItemsProcess(state);
  draw::DrawPopups(state);
}
}  // namespace esc::draw