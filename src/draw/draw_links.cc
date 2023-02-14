#include "draw_links.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include "imgui.h"

namespace esc::draw {
void Links::Draw(State& state) {
  for (const auto& link : state.app_.GetDiagram().GetLinks()) {
    // if (state.drawing_.not_yet_connected_pin_of_new_link_id.has_value()) {
    //   const auto pin_id =
    //   *state.drawing_.not_yet_connected_pin_of_new_link_id; const auto& node
    //   = state.app_.GetDiagram().FindPinNode(pin_id);

    //   // std::cout << node->GetId().Get() << "\n";

    //   if (pin_id == link.start_pin_id || pin_id == link.end_pin_id) {
    //     ne::Link(link.id, link.start_pin_id, link.end_pin_id,
    //              ImColor{255, 0, 0}, 0.0F);

    //     continue;
    //   }
    // }

    ne::Link(link.id, link.start_pin_id, link.end_pin_id,
             ImColor{255, 255, 255}, 2.0F);
  }

  if (!state.drawing_.not_yet_connected_pin_of_new_link_id.has_value()) {
    return;
  }

  const auto new_link_starts_from_pin =
      *state.drawing_.not_yet_connected_pin_of_new_link_id;
  const auto* existing_link_from_same_pin =
      state.app_.GetDiagram().FindLinkFromPin(new_link_starts_from_pin);

  if (existing_link_from_same_pin == nullptr) {
    return;
  }

  const auto existing_link_pinned_pin =
      (existing_link_from_same_pin->start_pin_id == new_link_starts_from_pin)
          ? existing_link_from_same_pin->end_pin_id
          : existing_link_from_same_pin->start_pin_id;

  const auto& pinned_node =
      state.app_.GetDiagram().FindPinNode(existing_link_pinned_pin);

  auto* drawList = ImGui::GetWindowDrawList();
  // ImGui::GetMousePos() - ImGui::GetMouseDragDelta();
  drawList->AddLine(pinned_node->GetPosition(), ImGui::GetMousePos(), ImColor{255, 0, 0}, 5);
}
}  // namespace esc::draw