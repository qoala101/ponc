#include "draw_link_connection_process.h"

#include <iostream>
#include <utility>

#include "app_state.h"
#include "core_link.h"
#include "coreui_i_node_drawer.h"
#include "cpp_scope.h"
#include "draw_tooltip.h"
#include "draw_widgets.h"
#include "imgui_node_editor.h"

namespace esc::draw {
namespace {
void UpdateNewLink(AppState& app_state, NewLink& new_link) {
  for (const auto& link : state.core_state->diagram_.GetLinks()) {
    if (link.start_pin_id == new_link.pin_dragged_from) {
      new_link.rebind.emplace(
          Rebind{link.end_pin_id, ne::PinKind::Input, link.id});
      break;
    }

    if (link.end_pin_id == new_link.pin_dragged_from) {
      new_link.rebind.emplace(
          Rebind{link.start_pin_id, ne::PinKind::Output, link.id});
      break;
    }
  }
}
}  // namespace

LinkConnectionProcess::LinkConnectionProcess(
    std::shared_ptr<std::optional<NewLink>> new_link)
    : new_link_{std::move(new_link)} {}

void LinkConnectionProcess::Draw(AppState& app_state) {
  const auto create_scope = cpp::Scope{[]() { ne::EndCreate(); }};
  auto alpha = 255;

  if (new_link_->has_value() && (*new_link_)->rebind.has_value()) {
    alpha = 0;
  }

  if (ne::BeginCreate(ImColor{255, 255, 255, alpha}, 2.0F)) {
    auto start_pin_id = ne::PinId{};
    auto end_pin_id = ne::PinId{};

    if (ne::QueryNewLink(&start_pin_id, &end_pin_id)) {
      auto& new_link = new_link_->emplace(NewLink{start_pin_id, end_pin_id});

      UpdateNewLink(state, new_link);

      const auto& start_node =
          state.core_state->diagram_.FindPinNode(start_pin_id);
      const auto& end_node = state.core_state->diagram_.FindPinNode(end_pin_id);

      auto start_pin_drawer = start_node->CreateDrawer(state.ToStateNoQueue())
                                  ->CreatePinDrawer(start_pin_id);
      auto end_pin_drawer = end_node->CreateDrawer(state.ToStateNoQueue())
                                ->CreatePinDrawer(end_pin_id);

      if (!CanConnectFromPinToPin(*state.core_state, *new_link_,
                                  new_link.pin_dragged_from,
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
            state.core_state->diagram_.EraseLink(
                new_link.rebind->rebinding_link_id);

            if (new_link.rebind->fixed_pin_kind == ne::PinKind::Input) {
              state.core_state->diagram_.EmplaceLink(core::Link{
                  state.core_state->id_generator_.GetNext<ne::LinkId>(),
                  new_link.pin_hovered_over->Get(),
                  new_link.rebind->fixed_pin});
            } else {
              state.core_state->diagram_.EmplaceLink(core::Link{
                  state.core_state->id_generator_.GetNext<ne::LinkId>(),
                  new_link.rebind->fixed_pin,
                  new_link.pin_hovered_over->Get()});
            }
          }
        } else {
          DrawTooltip("+ Create Link", {32, 45, 32, 180});

          if (ne::AcceptNewItem(ImColor{127, 255, 127}, 4.0F)) {
            state.core_state->diagram_.EmplaceLink(core::Link{
                state.core_state->id_generator_.GetNext<ne::LinkId>(),
                new_link.pin_dragged_from, *new_link.pin_hovered_over});
          }
        }
      }
    } else {
      (*new_link_)->pin_hovered_over.reset();
    }

    // if (ne::QueryNewNode(&end_pin_id)) {
    //   auto& new_link =
    //       new_link_->emplace(State::NewLink{end_pin_id});

    //   UpdateNewLink(state, new_link);

    //   Tooltip{"+ Create Node", {32, 45, 32, 180}}.Draw(state);

    //   if (ne::AcceptNewItem()) {
    //     const auto suspend_scope =
    //         cpp::Scope{[]() { ne::Suspend(); }, []() { ne::Resume(); }};

    //     ImGui::OpenPopup("Create New Node");
    //   }
    // }
  } else {
    new_link_->reset();
  }
}
}  // namespace esc::draw