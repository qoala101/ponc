#include "draw_new_link_popup.h"

#include <cstdint>
#include <iostream>
#include <limits>
#include <memory>
#include <ranges>
#include <string>
#include <vector>

#include "app_event_queue.h"
#include "app_events.h"
#include "core_diagram.h"
#include "core_i_family.h"
#include "core_i_node.h"
#include "core_id_generator.h"
#include "core_project.h"
#include "coreui_i_family_drawer.h"
#include "cpp_assert.h"
#include "draw_id_label.h"
#include "draw_widgets.h"
#include "imgui_node_editor.h"

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
//       (*state_)->id_generator_.GetNext<ne::LinkId>(),
//       *drawing_state_.connect_new_node_to_existing_pin_id,
//       new_node);
// }

// break;
}  // namespace

// ---
void NewLinkPopup::SetPosition(const ImVec2& position) { position_ = position; }

// ---
auto NewLinkPopup::GetLabel() const -> std::string { return "Connect Node"; }

// ---
void NewLinkPopup::SetDraggedFromPin(ne::PinId pin_id) {
  dragged_from_pin_ = pin_id;
}

// ---
void NewLinkPopup::DrawItems(const AppState& app_state) {
  const auto& families = app_state.project.GetFamilies();

  const auto dragged_from_node =
      core::FindPinNode(app_state.project.GetDiagram(), dragged_from_pin_);
  const auto dragged_from_pin_kind =
      core::GetPinKind(*dragged_from_node, dragged_from_pin_);

  auto families_with_suitable_pins =
      std::vector<std::shared_ptr<core::IFamily>>{};
  auto id_generator = core::IdGenerator{};

  for (const auto& family : families) {
    const auto node = family->CreateNode(id_generator);

    if ((dragged_from_pin_kind == ne::PinKind::Input) &&
        !node->GetOutputPinIds().empty()) {
      families_with_suitable_pins.emplace_back(family);
      continue;
    }

    if ((dragged_from_pin_kind == ne::PinKind::Output) &&
        node->GetInputPinId().has_value()) {
      families_with_suitable_pins.emplace_back(family);
      continue;
    }
  }

  const auto family_groups = coreui::GroupByLabels(families_with_suitable_pins);

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
              IdLabel(family->CreateDrawer()->GetLabel(), family->GetId())
                  .c_str())) {
        auto new_node = family->CreateNode(app_state.id_generator);
        auto connect_to_pin_id = ne::PinId{};

        if (dragged_from_pin_kind == ne::PinKind::Input) {
          const auto& output_pin_ids = new_node->GetOutputPinIds();
          Expects(!output_pin_ids.empty());
          connect_to_pin_id = output_pin_ids[0];
        } else {
          const auto& input_pin_id = new_node->GetInputPinId();
          Expects(input_pin_id.has_value());
          connect_to_pin_id = *input_pin_id;
        }

        app_state.event_queue.PostEvent(Events::EmplaceNode{
            .node = std::move(new_node), .position = position_});

        const auto new_link_event =
            (dragged_from_pin_kind == ne::PinKind::Input)
                ? Events::CreateLink{.start_pin_id = connect_to_pin_id,
                                     .end_pin_id = dragged_from_pin_}
                : Events::CreateLink{.start_pin_id = dragged_from_pin_,
                                     .end_pin_id = connect_to_pin_id};

        app_state.event_queue.PostEvent(new_link_event);
      }
    }

    if (!is_group) {
      continue;
    }

    ImGui::EndMenu();
  }
}
}  // namespace esc::draw