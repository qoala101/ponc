#ifndef VH_ESC_APP_IMPL_H_
#define VH_ESC_APP_IMPL_H_

#include "draw_delete_items_process.h"
#include "draw_groups.h"
#include "draw_i_family_drawer.h"
#include "draw_i_node_drawer.h"
#include "draw_i_pin_drawer.h"
#include "draw_link_connection_process.h"
#include "draw_links.h"
#include "draw_main_window.h"
#include "draw_nodes.h"
#include "draw_popups.h"
#include "draw_repin_curve.h"
#include "draw_tooltip.h"
#include "esc_editor_context_handle.h"
#include "esc_node_drawer.h"
#include "esc_state.h"
#include "esc_textures_handle.h"

namespace esc {

struct AppImpl {
  std::shared_ptr<State> state_{};
  esc::EditorContextHandle editor_context_{};
  draw::RepinCurve repin_curve_{};
  draw::Groups groups_{};
  draw::Nodes nodes_{};
  draw::MainWindow main_window_;
  draw::Popups popups_{};
  draw::Links links_{};
  draw::LinkConnectionProcess link_connection_process_{};
  draw::DeleteItemsProcess delete_items_process_{};
};
}

#endif  // VH_ESC_APP_IMPL_H_
