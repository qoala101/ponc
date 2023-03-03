#include "draw_background_popup.h"

#include <imgui.h>

#include <ranges>
#include <string>

namespace esc::draw {
///
void BackgroundPopup::Draw(
    const std::vector<coreui::FamilyGroup>& family_groups) {
  const auto [is_visible, content_scope] = DrawContentScope();

  if (!is_visible) {
    return;
  }

  for (const auto& family_group : family_groups) {
    const auto is_group = family_group.families.size() > 1;
    auto draw_items = true;

    if (is_group) {
      draw_items = ImGui::BeginMenu(family_group.label.c_str());
    }

    if (!draw_items) {
      continue;
    }

    for (const auto& family : family_group.families) {
      if (ImGui::MenuItem(family.GetLabel().c_str())) {
        family.CreateNodeAt(pos_);
      }
    }

    if (!is_group) {
      continue;
    }

    ImGui::EndMenu();
  }
}

///
void BackgroundPopup::SetPos(const ImVec2& pos) { pos_ = pos; }

///
auto BackgroundPopup::GetLabel() const -> std::string {
  return "Create New Node";
}
}  // namespace esc::draw