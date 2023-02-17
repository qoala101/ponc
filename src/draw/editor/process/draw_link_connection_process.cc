#include "draw_link_connection_process.h"

#include <iostream>

#include "core_link.h"
#include "coreui_i_node_drawer.h"
#include "cpp_scope.h"
#include "draw_tooltip.h"
#include "imgui_node_editor.h"

namespace esc::draw {
namespace {
void UpdateNewLink(State& state, State::NewLink& new_link) {
  for (const auto& link : state.app_.GetDiagram().GetLinks()) {
    if (link.start_pin_id == new_link.pin_dragged_from) {
      new_link.rebind.emplace(
          State::Rebind{link.end_pin_id, ne::PinKind::Input, link.id});
      break;
    }

    if (link.end_pin_id == new_link.pin_dragged_from) {
      new_link.rebind.emplace(
          State::Rebind{link.start_pin_id, ne::PinKind::Output, link.id});
      break;
    }
  }
}
}  // namespace

void DrawLinkConnectionProcess(State& state) {
  const auto create_scope = cpp::Scope{[]() { ne::EndCreate(); }};
  auto alpha = 255;

  if (state.drawing_.new_link.has_value() &&
      state.drawing_.new_link->rebind.has_value()) {
    alpha = 0;
  }

  if (ne::BeginCreate(ImColor{255, 255, 255, alpha}, 2.0F)) {
    auto start_pin_id = ne::PinId{};
    auto end_pin_id = ne::PinId{};

    if (ne::QueryNewLink(&start_pin_id, &end_pin_id)) {
      auto& new_link = state.drawing_.new_link.emplace(
          State::NewLink{start_pin_id, end_pin_id});

      UpdateNewLink(state, new_link);

      const auto& start_node =
          state.app_.GetDiagram().FindPinNode(start_pin_id);
      const auto& end_node = state.app_.GetDiagram().FindPinNode(end_pin_id);

      auto start_pin_drawer =
          start_node->CreateDrawer(state)->CreatePinDrawer(start_pin_id);
      auto end_pin_drawer =
          end_node->CreateDrawer(state)->CreatePinDrawer(end_pin_id);

      if (!state.CanConnectFromPinToPin(new_link.pin_dragged_from,
                                        *new_link.pin_hovered_over)) {
        auto alpha = 255;

        if (new_link.rebind.has_value()) {
          alpha = 0;
        }

        ne::RejectNewItem(ImColor{255, 0, 0, alpha}, 2.0F);
      } else {
        if (new_link.rebind.has_value()) {
          DrawTooltip("+ Move Link", {32, 45, 32, 180});

          if (ne::AcceptNewItem(ImColor{0, 0, 0, 0})) {
            ne::DeleteLink(new_link.rebind->rebinding_link_id);
            state.app_.GetDiagram().EraseLink(
                new_link.rebind->rebinding_link_id);

            if (new_link.rebind->fixed_pin_kind == ne::PinKind::Input) {
              state.app_.GetDiagram().EmplaceLink(
                  core::Link{state.id_generator_.GetNext<ne::LinkId>(),
                             new_link.pin_hovered_over->Get(),
                             new_link.rebind->fixed_pin});
            } else {
              state.app_.GetDiagram().EmplaceLink(
                  core::Link{state.id_generator_.GetNext<ne::LinkId>(),
                             new_link.rebind->fixed_pin,
                             new_link.pin_hovered_over->Get()});
            }
          }
        } else {
          DrawTooltip("+ Create Link", {32, 45, 32, 180});

          if (ne::AcceptNewItem(ImColor{127, 255, 127}, 4.0F)) {
            state.app_.GetDiagram().EmplaceLink(core::Link{
                state.id_generator_.GetNext<ne::LinkId>(),
                new_link.pin_dragged_from, *new_link.pin_hovered_over});
          }
        }
      }
    } else {
      state.drawing_.new_link->pin_hovered_over.reset();
    }

    // if (ne::QueryNewNode(&end_pin_id)) {
    //   auto& new_link =
    //       state.drawing_.new_link.emplace(State::NewLink{end_pin_id});

    //   UpdateNewLink(state, new_link);

    //   Tooltip{"+ Create Node", {32, 45, 32, 180}}.Draw(state);

    //   if (ne::AcceptNewItem()) {
    //     const auto suspend_scope =
    //         cpp::Scope{[]() { ne::Suspend(); }, []() { ne::Resume(); }};

    //     ImGui::OpenPopup("Create New Node");
    //   }
    // }
  } else {
    state.drawing_.new_link.reset();
  }
}
}  // namespace esc::draw