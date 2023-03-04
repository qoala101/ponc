#include "draw_background_connection_popup.h"

#include <cstdint>
#include <iostream>
#include <limits>
#include <memory>
#include <ranges>
#include <string>
#include <vector>

#include "core_i_node.h"
#include "coreui_family.h"
#include "imgui_node_editor.h"

namespace esc::draw {
///
auto BackgroundConnectionPopup::GetGroupsValidForConnection(
    const std::vector<coreui::FamilyGroup>& family_groups) const {
  const auto target_pin_kind =
      core::INode::GetOppositePinKind(source_pin_kind_);

  auto valid_groups =
      std::vector<std::pair<std::string, std::vector<const coreui::Family*>>>{};

  for (const auto& family_group : family_groups) {
    for (const auto& family : family_group.families) {
      if (!family.NodesHavePinOfKind(target_pin_kind)) {
        continue;
      }

      const auto valid_group =
          std::find_if(valid_groups.begin(), valid_groups.end(),
                       [&family_group](const auto& group) {
                         return group.first == family_group.label;
                       });

      if (valid_group == valid_groups.end()) {
        valid_groups.emplace_back(family_group.label, std::vector{&family});
        continue;
      }

      valid_group->second.emplace_back(&family);
    }
  }

  return valid_groups;
}

///
void BackgroundConnectionPopup::Draw(
    const std::vector<coreui::FamilyGroup>& family_groups) {
  const auto [is_visible, content_scope] = DrawContentScope();

  if (!is_visible) {
    return;
  }

  for (const auto& family_group : GetGroupsValidForConnection(family_groups)) {
    const auto& [group_label, families] = family_group;
    const auto is_group = families.size() > 1;
    auto draw_items = true;

    if (is_group) {
      draw_items = ImGui::BeginMenu(group_label.c_str());
    }

    if (!draw_items) {
      continue;
    }

    for (const auto& family : families) {
      if (ImGui::MenuItem(family->GetLabel().c_str())) {
        family->CreateNodeAtAndConnectTo(pos_, source_pin_id_,
                                         source_pin_kind_);
      }
    }

    if (!is_group) {
      continue;
    }

    ImGui::EndMenu();
  }
}

///
void BackgroundConnectionPopup::SetPos(const ImVec2& pos) { pos_ = pos; }

///
auto BackgroundConnectionPopup::GetLabel() const -> std::string {
  return "Connect Node";
}

///
void BackgroundConnectionPopup::SetSourcePin(ne::PinId pin_id,
                                             ne::PinKind pin_kind) {
  source_pin_id_ = pin_id;
  source_pin_kind_ = pin_kind;
}
}  // namespace esc::draw