#include "app_app_impl.h"
#include "app_state.h"
#include "cpp_scope.h"
#include "draw_delete_items_process.h"
#include "draw_groups.h"
#include "draw_link_being_repinned.h"
#include "draw_link_connection_process.h"
#include "draw_links.h"
#include "draw_main_window.h"
#include "draw_node_editor.h"
#include "draw_nodes.h"
#include "draw_popups.h"

namespace esc {
void AppImpl::OnFrame() {
  state_.OnFrame();
  draw::DrawMainWindow(state_);
  draw::DrawNodeEditor(state_);
}
}  // namespace esc