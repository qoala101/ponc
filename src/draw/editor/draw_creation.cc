#include "draw_creation.h"

#include <iostream>

#include "cpp_scope.h"
#include "draw_tooltip.h"
#include "frame_node.h"
#include "imgui_node_editor.h"

namespace esc::draw {
void Creation::Draw(
    const coreui::Creation& creation, std::optional<coreui::NewLink>& new_link,
    const SignalCreateCurrentLink& signal_create_current_link,
    const SignalCreateConnectedNode& signal_create_connected_node) {
  const auto popup_position = ImGui::GetMousePos();

  {
    if (ne::BeginCreate(ImColor{1.F, 1.F, 1.F, creation.alpha}, 3.F)) {
      auto dragged_from_pin = ne::PinId{};
      auto hovering_over_pin = ne::PinId{};

      if (ne::QueryNewLink(&dragged_from_pin, &hovering_over_pin)) {
        new_link = coreui::NewLink{.dragged_from_pin_ = dragged_from_pin,
                                   .hovering_over_pin_ = hovering_over_pin};

        const auto& reason = creation.reason;

        if (creation.can_connect) {
          if (!reason.empty()) {
            DrawTooltip(reason.c_str(),
                        ImColor{0.F, 1.F / 3, 0.F, 1.F * 3 / 4});

            if (ne::AcceptNewItem(
                    ImColor{1.F / 2, 1.F, 1.F / 2, creation.alpha}, 4.F)) {
              signal_create_current_link();
            }
          }
        } else {
          DrawTooltip(reason, ImColor{1.F / 3, 0.F, 0.F, 1.F * 3 / 4});
          ne::RejectNewItem(ImColor{1.F, 1.F / 2, 1.F / 2, creation.alpha},
                            4.F);
        }
      } else if (ne::QueryNewNode(&dragged_from_pin)) {
        new_link = coreui::NewLink{.dragged_from_pin_ = dragged_from_pin};

        DrawTooltip("Create Node", ImColor{0.F, 1.F / 3, 0.F, 1.F * 3 / 4});

        if (ne::AcceptNewItem()) {
          signal_create_connected_node(popup_position, dragged_from_pin);
        }
      }
    }

    ne::EndCreate();
  }
}
}  // namespace esc::draw