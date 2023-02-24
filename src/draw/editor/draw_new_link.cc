#include "draw_new_link.h"

#include <iostream>

#include "app_event_queue.h"
#include "app_events.h"
#include "cpp_scope.h"
#include "draw_tooltip.h"
#include "draw_widgets.h"
#include "frame_node.h"
#include "imgui_node_editor.h"

namespace esc::draw {
void NewLink::Draw(const AppState& app_state) {
  const auto popup_position = ImGui::GetMousePos();

  {
    const auto create_scope = cpp::Scope{[]() { ne::EndCreate(); }};

    if (ne::BeginCreate(ImColor{1.F, 1.F, 1.F, app_state.frame.creation_alpha},
                        3.F)) {
      auto dragged_from_pin = ne::PinId{};
      auto hovering_over_pin = ne::PinId{};

      if (ne::QueryNewLink(&dragged_from_pin, &hovering_over_pin)) {
        dragged_from_pin_ = dragged_from_pin;
        hovering_over_pin_ = hovering_over_pin;

        if (app_state.frame.creation.has_value()) {
          if (app_state.frame.creation->can_connect) {
            if (!app_state.frame.creation->reason.empty()) {
              DrawTooltip(app_state.frame.creation->reason.c_str(),
                          ImColor{0.F, 1.F / 3, 0.F, 1.F * 3 / 4});

              if (ne::AcceptNewItem(ImColor{1.F / 2, 1.F, 1.F / 2,
                                            app_state.frame.creation_alpha},
                                    4.F)) {
                if (app_state.frame.creation->delete_link.has_value()) {
                  app_state.event_queue.PostEvent(Events::DeleteLink{
                      .link_id = *app_state.frame.creation->delete_link});
                }

                app_state.event_queue.PostEvent(Events::CreateLink{
                    .start_pin_id = app_state.frame.creation->start_pin_id,
                    .end_pin_id = app_state.frame.creation->end_pin_id});
              }
            }
          } else {
            DrawTooltip(app_state.frame.creation->reason,
                        ImColor{1.F / 3, 0.F, 0.F, 1.F * 3 / 4});
            ne::RejectNewItem(
                ImColor{1.F, 1.F / 2, 1.F / 2, app_state.frame.creation_alpha},
                4.F);
          }
        }
      } else if (ne::QueryNewNode(&dragged_from_pin)) {
        dragged_from_pin_ = dragged_from_pin;
        hovering_over_pin_.reset();

        DrawTooltip("Create Node", ImColor{0.F, 1.F / 3, 0.F, 1.F * 3 / 4});

        if (ne::AcceptNewItem()) {
          const auto suspend_scope =
              cpp::Scope{[]() { ne::Suspend(); }, []() { ne::Resume(); }};

          new_link_popup.SetPosition(popup_position);
          new_link_popup.SetDraggedFromPin(*dragged_from_pin_);
          new_link_popup.Show();
        }
      } else {
        hovering_over_pin_.reset();
      }
    } else {
      dragged_from_pin_.reset();
      hovering_over_pin_.reset();
    }
  }

  new_link_popup.Draw(app_state);
}

auto NewLink::GetDraggedFromPin() const -> const std::optional<ne::PinId>& {
  return dragged_from_pin_;
}

auto NewLink::GetHoveringOverPin() const -> const std::optional<ne::PinId>& {
  return hovering_over_pin_;
}
}  // namespace esc::draw