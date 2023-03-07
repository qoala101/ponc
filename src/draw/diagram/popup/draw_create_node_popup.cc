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
  const auto [is_visible, content_scope] =
      DrawContentScope({.closed = callbacks.closed});

  if (!is_visible) {
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
      if (ImGui::MenuItem(family.GetLabel().c_str())) {
        callbacks.node_created(family.CreateNode());
      }
    }

    if (!is_group) {
      continue;
    }

    ImGui::EndMenu();
  }
}

///
auto CreateNodePopup::GetLabel() const -> std::string { return "Crate Node"; }
}  // namespace esc::draw