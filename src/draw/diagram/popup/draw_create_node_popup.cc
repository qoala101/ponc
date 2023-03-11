#include "draw_create_node_popup.h"

#include <cstdint>
#include <limits>
#include <memory>
#include <ranges>
#include <string>
#include <vector>

#include "coreui_family.h"

namespace esc::draw {
///
void CreateNodePopup::Draw(
    const std::vector<coreui::FamilyGroup>& family_groups,
    const Callbacks& callbacks) {
  const auto content_scope = DrawContentScope({.closed = callbacks.closed});

  if (!IsOpened()) {
    return;
  }

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
      const auto is_family_enabled =
          !callbacks.is_family_enabled.has_value() ||
          (*callbacks.is_family_enabled)(family.GetFamily());

      if (ImGui::MenuItem(family.GetLabel().c_str(), nullptr, false,
                          is_family_enabled)) {
        auto new_node = family.CreateNode();
        new_node->SetPos(pos_);

        callbacks.node_created(std::move(new_node));
      }
    }

    if (is_group) {
      ImGui::EndMenu();
    }
  }
}

///
void CreateNodePopup::SetPos(const ImVec2& pos) { pos_ = pos; }

///
auto CreateNodePopup::GetLabel() const -> std::string { return "Crate Node"; }
}  // namespace esc::draw