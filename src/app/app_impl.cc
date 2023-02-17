#include "app_impl.h"

#include "core_i_node.h"
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
  draw_state.node_header_texture = textures.node_header;
  ne::SetCurrentEditor(editor_context_);
}

AppImpl::~AppImpl() { ne::DestroyEditor(editor_context_); }

void AppImpl::OnFrame() {
  auto state_no_queue =
      StateNoQueue{.core_state = &core_state_, .draw_state = &draw_state};
  event_queue_.ExecuteEvents(state_no_queue);
  core_state_.flow_calculator_.OnFrame(core_state_);

  auto state = State{.core_state = &core_state_,
                     .draw_state = &draw_state,
                     .event_queue = &event_queue_};

  draw::DrawMainWindow(state);
  draw::DrawNodeEditor(state);
}
}  // namespace esc