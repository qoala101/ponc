#include "draw_links.h"

#include "imgui.h"
#include "imgui_node_editor.h"

namespace esc::draw {
void DrawLinks(State& state) {
  // const auto* existing_link_from_same_pin =
  // state.GetExistingLinkFromSamePin();

  for (const auto& link : state.core_state->diagram_.GetLinks()) {
    // if (state.draw_state->not_yet_connected_pin_of_new_link_id.has_value()) {
    //   const auto pin_id =
    //   *state.draw_state->not_yet_connected_pin_of_new_link_id; const auto&
    //   node = state.core_state->diagram_.FindPinNode(pin_id);

    //   // std::cout << node->GetId().Get() << "\n";

    //   if (pin_id == link.start_pin_id || pin_id == link.end_pin_id) {
    //     ne::Link(link.id, link.start_pin_id, link.end_pin_id,
    //              ImColor{255, 0, 0}, 0.0F);

    //     continue;
    //   }
    // }

    auto alpha = 1.0F;

    if (state.draw_state->new_link.has_value()) {
      if (state.draw_state->new_link->rebind.has_value()) {
        if (state.draw_state->new_link->rebind->rebinding_link_id == link.id) {
          alpha = 0.25F;
        }
      }
    }

    // if (&link != existing_link_from_same_pin) {
    const auto& node =
        state.core_state->diagram_.FindPinNode(link.start_pin_id);
    const auto node_flow = state.core_state->flow_calculator_.GetCalculatedFlow(*node);
    const auto flow = GetPinFlow(node_flow, link.start_pin_id);

    auto color = state.core_state->flow_colors_.GetColorForFlowValue(flow);
    color.Value.w = alpha;

    ne::Link(link.id, link.start_pin_id, link.end_pin_id, color, 2.0F);

    // ne::Flow(link.id);
    // }
  }
}
}  // namespace esc::draw