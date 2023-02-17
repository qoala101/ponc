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
#include "imgui_node_editor.h"

namespace esc {
AppImpl::AppImpl(const Textures &textures)
    : editor_context_{ne::CreateEditor()} {
  state_.textures_ = textures;
  ne::SetCurrentEditor(editor_context_);
}

AppImpl::~AppImpl() { ne::DestroyEditor(editor_context_); }

void AppImpl::OnFrame() {
  state_.OnFrame();
  draw::DrawMainWindow(state_);
  draw::DrawNodeEditor(state_);
}
}  // namespace esc