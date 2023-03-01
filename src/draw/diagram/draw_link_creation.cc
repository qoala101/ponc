#include "draw_link_creation.h"

#include <iostream>

#include "coreui_project.h"
#include "cpp_scope.h"
#include "draw_tooltip.h"
#include "imgui_node_editor.h"

namespace esc::draw {
void LinkCreation::Draw(
    coreui::LinkCreation& creation,
    const SignalCreateCurrentLink& signal_create_current_link,
    const SignalCreateConnectedNode& signal_create_connected_node) {
  const auto new_link_alpha = creation.IsRepinningLink() ? 0.F : 1.F;
  const auto popup_position = ImGui::GetMousePos();

  {
    if (ne::BeginCreate(ImColor{1.F, 1.F, 1.F, new_link_alpha}, 3.F)) {
      auto dragged_from_pin = ne::PinId{};
      auto hovering_over_pin = ne::PinId{};

      if (ne::QueryNewLink(&dragged_from_pin, &hovering_over_pin)) {
        creation.SetPins(dragged_from_pin, hovering_over_pin);

        const auto& [can_connect, reason] = creation.GetCanCreateLinkReason();

        if (can_connect) {
          if (!reason.empty()) {
            DrawTooltip(reason.c_str(),
                        ImColor{0.F, 1.F / 3, 0.F, 1.F * 3 / 4});

            if (ne::AcceptNewItem(
                    ImColor{1.F / 2, 1.F, 1.F / 2, new_link_alpha}, 4.F)) {
              creation.AcceptCurrentLink();
            }
          }
        } else {
          DrawTooltip(reason, ImColor{1.F / 3, 0.F, 0.F, 1.F * 3 / 4});
          ne::RejectNewItem(ImColor{1.F, 1.F / 2, 1.F / 2, new_link_alpha},
                            4.F);
        }
      } else if (ne::QueryNewNode(&dragged_from_pin)) {
        creation.SetPins(dragged_from_pin);

        DrawTooltip("Create Node", ImColor{0.F, 1.F / 3, 0.F, 1.F * 3 / 4});

        if (ne::AcceptNewItem()) {
          signal_create_connected_node(popup_position, dragged_from_pin);
        }
      }
    } else {
      creation.SetPins({});
    }

    ne::EndCreate();
  }
}
}  // namespace esc::draw