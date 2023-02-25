#include "draw_new_link.h"

#include <iostream>

#include "app_event_queue.h"
#include "cpp_scope.h"
#include "draw_tooltip.h"
#include "frame_node.h"
#include "imgui_node_editor.h"

namespace esc::draw {
void NewLink::Draw(frame::Frame& frame) {
  const auto popup_position = ImGui::GetMousePos();

  {
    const auto create_scope = cpp::Scope{[]() { ne::EndCreate(); }};

    if (ne::BeginCreate(ImColor{1.F, 1.F, 1.F, frame.creation_alpha}, 3.F)) {
      auto dragged_from_pin = ne::PinId{};
      auto hovering_over_pin = ne::PinId{};

      if (ne::QueryNewLink(&dragged_from_pin, &hovering_over_pin)) {
        frame.new_link.dragged_from_pin_ = dragged_from_pin;
        frame.new_link.hovering_over_pin_ = hovering_over_pin;

        if (frame.creation.has_value()) {
          if (frame.creation->can_connect) {
            if (!frame.creation->reason.empty()) {
              DrawTooltip(frame.creation->reason.c_str(),
                          ImColor{0.F, 1.F / 3, 0.F, 1.F * 3 / 4});

              if (ne::AcceptNewItem(
                      ImColor{1.F / 2, 1.F, 1.F / 2, frame.creation_alpha},
                      4.F)) {
                if (frame.creation->delete_link.has_value()) {
                  frame.DeleteLink(*frame.creation->delete_link);
                }

                frame.CreateLink(frame.creation->start_pin_id,
                                 frame.creation->end_pin_id);
              }
            }
          } else {
            DrawTooltip(frame.creation->reason,
                        ImColor{1.F / 3, 0.F, 0.F, 1.F * 3 / 4});
            ne::RejectNewItem(
                ImColor{1.F, 1.F / 2, 1.F / 2, frame.creation_alpha}, 4.F);
          }
        }
      } else if (ne::QueryNewNode(&dragged_from_pin)) {
        frame.new_link.dragged_from_pin_ = dragged_from_pin;
        frame.new_link.hovering_over_pin_.reset();

        DrawTooltip("Create Node", ImColor{0.F, 1.F / 3, 0.F, 1.F * 3 / 4});

        if (ne::AcceptNewItem()) {
          const auto suspend_scope =
              cpp::Scope{[]() { ne::Suspend(); }, []() { ne::Resume(); }};

          new_link_popup.SetPosition(popup_position);
          new_link_popup.SetDraggedFromPin(*frame.new_link.dragged_from_pin_);
          new_link_popup.Show();
        }
      } else {
        frame.new_link.hovering_over_pin_.reset();
      }
    } else {
      frame.new_link.dragged_from_pin_.reset();
      frame.new_link.hovering_over_pin_.reset();
    }
  }

  new_link_popup.Draw(frame);
}
}  // namespace esc::draw