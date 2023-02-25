#include "draw_background_popup.h"

#include <ranges>
#include <string>

#include "core_i_family.h"
#include "core_project.h"
#include "coreui_i_family_traits.h"
#include "draw_id_label.h"
#include "frame_node.h"

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
//       (*state_)->diagram_.FindPin(pin_id, **state_)->GetKind() ==
//       ne::PinKind::Output;
//   const auto link = core::Link{
//       .id = link_id,
//       .start_pin_id =
//           is_link_starts_on_existing_node ? pin_id : *matching_node_pin_id,
//       .end_pin_id =
//           is_link_starts_on_existing_node ? *matching_node_pin_id : pin_id};

//   (*state_)->diagram_.EmplaceLink(link);
// }

// if (const auto node_created_by_link_from_existing_one =
//         drawing_state_.connect_new_node_to_existing_pin_id
//             .has_value()) {
//   AddLinkFromPinToNode(
//       (*state_)->id_generator_.Generate<ne::LinkId>(),
//       *drawing_state_.connect_new_node_to_existing_pin_id,
//       new_node);
// }

// break;
}  // namespace

void BackgroundPopup::SetPosition(const ImVec2& position) {
  position_ = position;
}

auto BackgroundPopup::GetLabel() const -> std::string {
  return "Create New Node";
}

void BackgroundPopup::DrawItems(frame::Frame& frame) {
  const auto family_groups =
      coreui::GroupByLabels(frame.GetProject().GetFamilies());

  for (const auto& [group_label, families] : family_groups) {
    const auto is_group = families.size() > 1;
    auto draw_items = true;

    if (is_group) {
      draw_items = ImGui::BeginMenu(group_label.c_str());
    }

    if (!draw_items) {
      continue;
    }

    for (const auto& family : families) {
      if (ImGui::MenuItem(
              IdLabel(family->CreateUiTraits()->GetLabel(), family->GetId())
                  .c_str())) {
        frame.EmplaceNode(family->CreateNode(frame.GetIdGenerator()),
                          position_);
      }
    }

    if (!is_group) {
      continue;
    }

    ImGui::EndMenu();
  }
}
}  // namespace esc::draw