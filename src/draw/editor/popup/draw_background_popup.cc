#include "draw_background_popup.h"

#include <ranges>
#include <string>

#include "core_i_family.h"
#include "coreui_i_family_traits.h"
#include "draw_id_label.h"

namespace esc::draw {
void BackgroundPopup::SetPosition(const ImVec2& position) {
  position_ = position;
}

auto BackgroundPopup::GetLabel() const -> std::string {
  return "Create New Node";
}

void BackgroundPopup::Draw(
    const std::vector<std::shared_ptr<core::IFamily>>& families,
    const SignalCreateNode& signal_create_node) {
  const auto [is_visible, content_scope] = DrawContentScope();

  if (!is_visible) {
    return;
  }

  const auto family_groups = coreui::GroupByLabels(families);

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
        signal_create_node(family, position_);
      }
    }

    if (!is_group) {
      continue;
    }

    ImGui::EndMenu();
  }
}
}  // namespace esc::draw