#include "draw_background_popup.h"

#include <ranges>

#include "core_i_family.h"
#include "cpp_scope.h"
#include "draw_i_family_drawer.h"

namespace esc::draw {
namespace {
// void AddLinkFromPinToNode(ne::LinkId link_id, ne::PinId pin_id,
//                                core::INode& node) {
//   const auto& node_pin_ids = node.GetPinIds();
//   const auto matching_node_pin_id =
//       std::ranges::find_if(node_pin_ids, [this, pin_id](auto node_pin_id) {
//         return CanCreateLink(pin_id, node_pin_id);
//       });

//   if (matching_node_pin_id == node_pin_ids.end()) {
//     return;
//   }

//   const auto is_link_starts_on_existing_node =
//       (*state_)->app_.GetDiagram().FindPin(pin_id, **state_)->GetKind() ==
//       ne::PinKind::Output;
//   const auto link = core::Link{
//       .id = link_id,
//       .start_pin_id =
//           is_link_starts_on_existing_node ? pin_id : *matching_node_pin_id,
//       .end_pin_id =
//           is_link_starts_on_existing_node ? *matching_node_pin_id : pin_id};

//   (*state_)->app_.GetDiagram().EmplaceLink(link);
// }
}  // namespace

void DrawBackgroundPopup(State& state) {
  if (ImGui::BeginPopup("Create New Node")) {
    const auto popup_scope = cpp::Scope{[]() { ImGui::EndPopup(); }};

    ImGui::TextUnformatted("Create New Node");
    ImGui::Separator();

    const auto& families = state.app_.GetDiagram().GetFamilies();
    const auto family_groups = GroupByLabels(families);

    for (const auto& [group_label, families] : family_groups) {
      auto draw_items = true;
      const auto is_group = families.size() > 1;

      if (is_group) {
        draw_items = ImGui::BeginMenu(group_label.c_str());
      }

      if (draw_items) {
        for (const auto& family : families) {
          if (ImGui::MenuItem(family->CreateDrawer()->GetLabel().c_str())) {
            auto& new_node = family->EmplaceNode(state.id_generator_);

            new_node.SetPosition(state.DRAW_.popup_position);

            // if (const auto node_created_by_link_from_existing_one =
            //         drawing_state_.connect_new_node_to_existing_pin_id
            //             .has_value()) {
            //   AddLinkFromPinToNode(
            //       (*state_)->id_generator_.GetNext<ne::LinkId>(),
            //       *drawing_state_.connect_new_node_to_existing_pin_id,
            //       new_node);
            // }

            // break;
          }
        }

        if (is_group) {
          ImGui::EndMenu();
        }
      }
    }
  }
}
}  // namespace esc::draw