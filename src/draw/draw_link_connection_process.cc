#include "draw_link_connection_process.h"

#include <iostream>

#include "cpp_scope.h"
#include "draw_i_node_drawer.h"
#include "draw_tooltip.h"
#include "imgui_node_editor.h"

namespace esc::draw {
void LinkConnectionProcess::Draw(State& state) {
  const auto create_scope = cpp::Scope{[]() { ne::EndCreate(); }};

  if (ne::BeginCreate(ImColor{255, 255, 255}, 2.0F)) {
    auto start_pin_id = ne::PinId{};
    auto end_pin_id = ne::PinId{};

    if (ne::QueryNewLink(&start_pin_id, &end_pin_id)) {
      state.drawing_.not_yet_connected_pin_of_new_link_id = start_pin_id;

      const auto& start_node =
          state.app_.GetDiagram().FindPinNode(start_pin_id);
      const auto& end_node = state.app_.GetDiagram().FindPinNode(end_pin_id);

      auto start_pin_drawer =
          start_node->CreateDrawer(state)->CreatePinDrawer(start_pin_id);
      auto end_pin_drawer =
          end_node->CreateDrawer(state)->CreatePinDrawer(end_pin_id);

      if (end_pin_id == start_pin_id) {
        ne::RejectNewItem(ImColor{255, 0, 0}, 2.0F);
      } else if (end_pin_drawer->GetKind() == start_pin_drawer->GetKind()) {
        Tooltip{"x Incompatible Pin Kind", {45, 32, 32, 180}}.Draw(state);
        ne::RejectNewItem(ImColor{255, 0, 0}, 2.0F);
      } else if (start_node == end_node) {
        Tooltip{"x Cannot connect to self", {45, 32, 32, 180}}.Draw(state);
        ne::RejectNewItem(ImColor{255, 0, 0}, 1.0F);
        // } else if (end_pin->GetType() != start_pin_drawer->GetType()) {
        //   DrawHintLabel("x Incompatible Pin Type", ImColor{45, 32, 32,
        //   180});
        ne::RejectNewItem(ImColor{255, 127, 127}, 1.0F);
      } else {
        Tooltip{"+ Create Link", {32, 45, 32, 180}}.Draw(state);

        if (ne::AcceptNewItem(ImColor{127, 255, 127}, 4.0F)) {
          // if (start_pin_drawer->GetKind() == ne::PinKind::Input) {
          //   using std::swap;

          //   swap(start_pin_drawer, end_pin_drawer);
          //   swap(start_pin_id, end_pin_id);
          // }

          // state.app_.GetDiagram().EmplaceLink(
          //     core::Link{state.id_generator_.GetNext<ne::LinkId>(),
          //                start_pin_id, end_pin_id});
        }
      }
    }

    if (ne::QueryNewNode(&end_pin_id)) {
      state.drawing_.not_yet_connected_pin_of_new_link_id = end_pin_id;

      Tooltip{"+ Create Node", {32, 45, 32, 180}}.Draw(state);

      if (ne::AcceptNewItem()) {
        state.drawing_.connect_new_node_to_existing_pin_id =
            state.drawing_.not_yet_connected_pin_of_new_link_id;
        state.drawing_.not_yet_connected_pin_of_new_link_id.reset();

        {
          const auto suspend_scope =
              cpp::Scope{[]() { ne::Suspend(); }, []() { ne::Resume(); }};

          ImGui::OpenPopup("Create New Node");
        }
      }
    }
  } else {
    state.drawing_.not_yet_connected_pin_of_new_link_id.reset();
  }
}
}  // namespace esc::draw