/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_edit_link_popup.h"

#include <imgui.h>
#include <imgui_node_editor.h>

#include <algorithm>
#include <memory>
#include <regex>
#include <string>
#include <utility>
#include <vector>

#include "core_diagram.h"
#include "core_link.h"
#include "draw_disable_if.h"

namespace vh::ponc::draw {
///
void EditLinkPopup::Draw(coreui::Diagram& diagram) {
  const auto content_scope = DrawContentScope();

  if (!IsOpened()) {
    return;
  }

  auto& link = core::Diagram::FindLink(diagram.GetDiagram(), link_ids_.front());

  ImGui::InputFloat("Length", &link.length);

  const char* items[] = {"Default", "Custom"};
  static int item_current = 0;
  ImGui::Combo("Connection", &item_current, items, 2);

  {
    const auto disable_scope = DisableIf(true);
    ImGui::InputFloat("Attenuation/Length", &link.length);
    ImGui::InputFloat("Attenuation Add", &link.length);
  }

  if (ImGui::Button("Cancel")) {
    link = link_copies_.front();
    ImGui::CloseCurrentPopup();
  }
}

///
void EditLinkPopup::SetLinkIds(std::vector<ne::LinkId> link_ids) {
  link_ids_ = std::move(link_ids);
}
}  // namespace vh::ponc::draw