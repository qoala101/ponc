#include "draw_family_groups_menu.h"

#include <unordered_set>

#include "core_i_family.h"
#include "core_id_value.h"

namespace vh::ponc::draw {
///
auto FamilyGroupsMenu::GetDisabledFamilies(
    const std::vector<coreui::Family>& families, const Callbacks& callbacks) {
  if (!callbacks.is_family_enabled.has_value()) {
    return std::unordered_set<core::IdValue<core::FamilyId>>{};
  }

  auto disabled_families = std::unordered_set<core::IdValue<core::FamilyId>>{};

  for (const auto& family : families) {
    const auto& core_family = family.GetFamily();

    if (!(*callbacks.is_family_enabled)(core_family)) {
      disabled_families.insert(core_family.GetId().Get());
    }
  }

  return disabled_families;
}

///
void FamilyGroupsMenu::Draw(
    const std::vector<coreui::FamilyGroup>& family_groups,
    const Callbacks& callbacks) {
  for (const auto& [group_label, families] : family_groups) {
    const auto disabled_families = GetDisabledFamilies(families, callbacks);
    const auto is_group = families.size() > 1;
    auto draw_items = true;

    if (is_group) {
      const auto is_group_enabled = families.size() > disabled_families.size();
      draw_items = ImGui::BeginMenu(group_label.c_str(), is_group_enabled);
    }

    if (!draw_items) {
      continue;
    }

    for (const auto& family : families) {
      const auto is_family_enabled =
          !disabled_families.contains(family.GetFamily().GetId().Get());

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
}  // namespace vh::ponc::draw