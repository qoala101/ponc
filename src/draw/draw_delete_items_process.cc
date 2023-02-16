#include "draw_delete_items_process.h"

#include <iostream>

#include "core_link.h"
#include "cpp_scope.h"
#include "draw_i_node_drawer.h"
#include "draw_tooltip.h"
#include "imgui_node_editor.h"

namespace esc::draw {
void DeleteItemsProcess::Draw(State& state) {
  const auto delete_scope = cpp::Scope{[]() { ne::EndDelete(); }};

  if (ne::BeginDelete()) {
    auto link_id = ne::LinkId{};

    while (ne::QueryDeletedLink(&link_id)) {
      if (ne::AcceptDeletedItem()) {
      }
    }

    auto node_id = ne::NodeId{};

    while (ne::QueryDeletedNode(&node_id)) {
      if (ne::AcceptDeletedItem()) {
      }
    }
  }
}
}  // namespace esc::draw