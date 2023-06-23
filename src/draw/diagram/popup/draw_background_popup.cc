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
#include "draw_disable_if.h"
#include "draw_family_groups_menu.h"

namespace vh::ponc::draw {
// ///
// auto BackgroundPopup::DrawRenamePopup() {
//   auto rename_confirmed = false;

//   if (focus_name_input_) {
//     ImGui::SetKeyboardFocusHere();
//   }

//   const auto name_is_empty = area_name_buffer_.AsTrimmed().empty();

//   if (ImGui::InputText("##Diagram Name", area_name_buffer_.GetData(),
//                        area_name_buffer_.GetSize(),
//                        ImGuiInputTextFlags_AutoSelectAll |
//                            ImGuiInputTextFlags_EnterReturnsTrue)) {
//     if (!name_is_empty) {
//       rename_confirmed = true;
//     } else {
//       ImGui::SetKeyboardFocusHere(-1);
//     }
//   }

//   ImGui::SameLine();

//   {
//     const auto disable_scope = DisableIf(name_is_empty);

//     if (ImGui::Button("OK")) {
//       rename_confirmed = true;
//     }
//   }

//   if (rename_confirmed) {
//     ImGui::CloseCurrentPopup();
//   }

//   focus_name_input_ = false;
//   return rename_confirmed;
// }

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

  // ImGui::Separator();

  // if (ImGui::BeginMenu("Area")) {
  //   if (const auto rename_confirmed = DrawRenamePopup()) {
  //     diagram.CreateArea("Area", pos_);
  //   }

  //   ImGui::EndMenu();
  // }
}

///
void BackgroundPopup::SetPos(const ImVec2& pos) { pos_ = pos; }
}  // namespace vh::ponc::draw