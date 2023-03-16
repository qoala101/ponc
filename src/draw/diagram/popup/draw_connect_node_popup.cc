#include "draw_connect_node_popup.h"

#include <cstdint>
#include <limits>
#include <memory>
#include <string>
#include <vector>

#include "coreui_family.h"
#include "coreui_linker.h"
#include "draw_family_groups_menu.h"
#include "imgui.h"

namespace esc::draw {
///
void ConnectNodePopup::Draw(
    coreui::Linker& linker,
    const std::vector<coreui::FamilyGroup>& family_groups) {
  const auto content_scope = DrawContentScope(
      "Connect Node", {.closed = [&for_all_pins = for_all_pins_, &linker]() {
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
}  // namespace esc::draw