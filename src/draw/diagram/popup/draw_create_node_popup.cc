/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_create_node_popup.h"

#include <imgui.h>

#include <cstdint>
#include <limits>
#include <memory>
#include <string>
#include <vector>

#include "coreui_diagram.h"
#include "coreui_family.h"
#include "draw_family_groups_menu.h"

namespace vh::ponc::draw {
///
void CreateNodePopup::Draw(coreui::Diagram& diagram) {
  const auto content_scope = DrawContentScope("Create Node");

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
}

///
void CreateNodePopup::SetPos(const ImVec2& pos) { pos_ = pos; }
}  // namespace vh::ponc::draw