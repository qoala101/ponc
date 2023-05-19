/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_connect_node_popup.h"

#include <imgui.h>

#include <string>
#include <vector>

#include "coreui_family.h"
#include "coreui_linker.h"
#include "cpp_callbacks.h"
#include "draw_family_groups_menu.h"

namespace vh::ponc::draw {
///
auto ConnectNodePopup::GetLabel() -> std::string { return "Connect Node"; }

///
void ConnectNodePopup::Draw(
    coreui::Linker& linker,
    const std::vector<coreui::FamilyGroup>& family_groups) {
  const auto content_scope = DrawContentScope(
      GetLabel(), {.closed = [&for_all_pins = for_all_pins_, &linker]() {
        linker.DiscardCreateNode();
        for_all_pins = false;
      }});

  if (!IsOpened()) {
    return;
  }

  FamilyGroupsMenu::Draw(
      family_groups,
      {.is_family_enabled =
           [&linker](const auto& family) {
             return linker.CanConnectToFamily(family);
           },
       .family_selected =
           [&linker](const auto& family) { linker.AcceptCreateNode(family); }});

  if (!linker.CanCreateNodeForAllPins()) {
    return;
  }

  ImGui::Separator();

  if (ImGui::Checkbox("For All Pins", &for_all_pins_)) {
    linker.SetCreateNodeForAllPins(for_all_pins_);
  }
}
}  // namespace vh::ponc::draw