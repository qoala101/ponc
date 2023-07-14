/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_edit_link_popup.h"

#include <imgui.h>
#include <imgui_node_editor.h>

#include <algorithm>
#include <iterator>
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

  auto& core_diagram = diagram.GetDiagram();

  if (WasJustOpened()) {
    CopyLinks(core_diagram);
  }

  auto& link = link_copies_.front();

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
    Cancel(core_diagram);
    ImGui::CloseCurrentPopup();
  }
}

///
void EditLinkPopup::SetLinkIds(std::vector<ne::LinkId> link_ids) {
  link_ids_ = std::move(link_ids);
}

///
void EditLinkPopup::Cancel(core::Diagram& diagram) {
  for (const auto& link_copy : link_copies_) {
    auto& link = core::Diagram::FindLink(diagram, link_copy.id);
    link = link_copy;
  }
}

///
void EditLinkPopup::CopyLinks(const core::Diagram& diagram) {
  link_copies_.clear();
  link_copies_.reserve(link_ids_.size());

  std::transform(link_ids_.cbegin(), link_ids_.cend(),
                 std::back_inserter(link_copies_),
                 [&diagram](const auto link_id) {
                   return core::Diagram::FindLink(diagram, link_id);
                 });
}
}  // namespace vh::ponc::draw