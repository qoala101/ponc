#include "draw_creation_popup.h"

#include <cstdint>
#include <iostream>
#include <limits>
#include <memory>
#include <ranges>
#include <string>
#include <vector>

#include "core_diagram.h"
#include "core_i_family.h"
#include "core_i_node.h"
#include "core_id_generator.h"
#include "core_project.h"
#include "coreui_i_family_traits.h"
#include "cpp_assert.h"
#include "draw_id_label.h"
#include "frame_node.h"
#include "imgui_node_editor.h"

namespace esc::draw {
void CreationPopup::SetPosition(const ImVec2& position) {
  position_ = position;
}

auto CreationPopup::GetLabel() const -> std::string { return "Connect Node"; }

void CreationPopup::SetDraggedFromPin(ne::PinId pin_id) {
  dragged_from_pin_ = pin_id;
}

void CreationPopup::Draw(coreui::Frame& frame) {
  const auto [is_visible, content_scope] = DrawContentScope();

  if (!is_visible) {
    return;
  }

  const auto& families = frame.GetProject().GetFamilies();

  const auto dragged_from_node =
      core::FindPinNode(frame.GetProject().GetDiagram(), dragged_from_pin_);
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
              IdLabel(family->CreateUiTraits()->GetLabel(), family->GetId())
                  .c_str())) {
        auto new_node = family->CreateNode(frame.GetProject().GetIdGenerator());
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

        frame.EmplaceNode(std::move(new_node), position_);

        if (dragged_from_pin_kind == ne::PinKind::Input) {
          frame.CreateLink(connect_to_pin_id, dragged_from_pin_);
        } else {
          frame.CreateLink(dragged_from_pin_, connect_to_pin_id);
        }
      }
    }

    if (!is_group) {
      continue;
    }

    ImGui::EndMenu();
  }
}
}  // namespace esc::draw