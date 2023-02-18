#include "draw_state.h"

#include <iostream>

#include "app_state.h"
#include "core_state.h"
#include "coreui_i_node_drawer.h"
#include "draw_links.h"
#include "draw_nodes.h"

namespace esc::draw {
DrawState::DrawState(const Texture &node_header_texture)
    : DrawState(node_header_texture,
                std::make_shared<std::optional<NewLink>>()) {}

DrawState::DrawState(const Texture &node_header_texture,
                     std::shared_ptr<std::optional<NewLink>> new_link)
    : nodes{node_header_texture, new_link},
      links{new_link},
      link_being_repinned{new_link},
      link_connection_process{std::move(new_link)} {}
}  // namespace esc::draw