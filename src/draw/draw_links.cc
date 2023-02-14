#include "draw_links.h"

#include "imgui.h"

namespace esc::draw {
void Links::Draw(State& state) {
  const auto* existing_link_from_same_pin = state.GetExistingLinkFromSamePin();

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

    if (&link != existing_link_from_same_pin) {
      ne::Link(link.id, link.start_pin_id, link.end_pin_id,
               ImColor{255, 255, 255}, 2.0F);
    }
  }
}
}  // namespace esc::draw