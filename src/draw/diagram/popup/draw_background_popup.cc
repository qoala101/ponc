/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_background_popup.h"

#include <imgui.h>

#include <utility>

#include "coreui_diagram.h"
#include "coreui_family.h"
#include "draw_family_groups_menu.h"

namespace vh::ponc::draw {
///
void BackgroundPopup::Draw(coreui::Diagram& diagram) {
  const auto content_scope = DrawContentScope("Create");

  if (!IsOpened()) {
    return;
  }

  FamilyGroupsMenu::Draw(
      diagram.GetFamilyGroups(),
      {.family_selected = [&diagram, pos = pos_](const auto& family) {
        auto new_node = family.CreateNode();
        new_node->SetPos(pos);
        diagram.AddNode(std::move(new_node));
      }});

  ImGui::Separator();

  if (ImGui::MenuItem("Area")) {
  }
}

///
void BackgroundPopup::SetPos(const ImVec2& pos) { pos_ = pos; }
}  // namespace vh::ponc::draw