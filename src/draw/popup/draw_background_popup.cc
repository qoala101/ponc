#include "draw_background_popup.h"

#include "cpp_scope.h"
#include "draw_i_family_drawer.h"

namespace esc::draw {
BackgroundPopup::BackgroundPopup(const ImVec2& position)
    : position_{position} {}

auto BackgroundPopup::GetLabel(State& state) const -> std::string {
  return "Create New Node";
}

void BackgroundPopup::DrawContent(State& state) {
  const auto& families = state.app_.GetDiagram().GetFamilies();
  const auto family_groups = [&families]() {
    auto family_groups = std::map<std::string, std::vector<core::IFamily*>>{};

    for (const auto& family : families) {
      family_groups[family->CreateDrawer()->GetGroupLabel()].emplace_back(
          &*family);
    }

    return family_groups;
  }();

  for (const auto& [group_label, families] : family_groups) {
    auto draw_items = true;
    const auto is_group = families.size() > 1;

    if (is_group) {
      draw_items = ImGui::BeginMenu(group_label.c_str());
    }

    if (draw_items) {
      for (const auto& family : families) {
        if (ImGui::MenuItem(family->CreateDrawer()->GetLabel().c_str())) {
          SetVisible(false);

          auto& new_node = family->EmplaceNode(state.id_generator_);

          new_node.SetPosition(position_);

          // if (const auto node_created_by_link_from_existing_one =
          //         drawing_state_.connect_new_node_to_existing_pin_id
          //             .has_value()) {
          //   AddLinkFromPinToNode(
          //       (*state_)->id_generator_.GetNext<ne::LinkId>(),
          //       *drawing_state_.connect_new_node_to_existing_pin_id,
          //       new_node);
          // }

          // break;
        }
      }

      if (is_group) {
        ImGui::EndMenu();
      }
    }
  }
}
}  // namespace esc::draw