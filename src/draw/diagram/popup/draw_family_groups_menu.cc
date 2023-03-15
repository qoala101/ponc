#include "draw_family_groups_menu.h"

namespace esc::draw {
///
void FamilyGroupsMenu::Draw(
    const std::vector<coreui::FamilyGroup>& family_groups,
    const Callbacks& callbacks) {
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
        callbacks.family_selected(family);
      }
    }

    if (is_group) {
      ImGui::EndMenu();
    }
  }
}
}  // namespace esc::draw