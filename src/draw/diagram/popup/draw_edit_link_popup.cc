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

namespace vh::ponc::draw {
namespace {
///
auto IsSame(const std::vector<core::Link*>& links, const auto& comparator) {
  if (links.empty()) {
    return true;
  }

  auto* first_link = links.front();

  return std::all_of(links.cbegin(), links.cend(),
                     [first_link, &comparator](const auto* link) {
                       return comparator(*link, *first_link);
                     });
}

///
template <typename T>
auto IsSame(const std::vector<core::Link*>& links, T core::Link::*field) {
  return IsSame(links, [field](const auto& left, const auto& right) {
    return left.*field == right.*field;
  });
}
}  // namespace

///
auto EditLinkPopup::FindLinks(core::Diagram& diagram) const {
  auto links = std::vector<core::Link*>{};
  links.reserve(link_ids_.size());

  std::transform(link_ids_.cbegin(), link_ids_.cend(),
                 std::back_inserter(links), [&diagram](const auto link_id) {
                   return &core::Diagram::FindLink(diagram, link_id);
                 });
  return links;
}

///
void EditLinkPopup::Draw(coreui::Diagram& diagram,
                         const std::vector<core::Connection>& connections) {
  const auto content_scope = DrawContentScope();

  if (!IsOpened()) {
    return;
  }

  auto& core_diagram = diagram.GetDiagram();
  const auto links = FindLinks(core_diagram);

  if (WasJustOpened()) {
    CopyLinksAndConnections(links, connections);
  }

  auto* first_link = links.front();

  const auto length_edited =
      IsSame(links, &core::Link::length)
          ? ImGui::InputFloat("Length", &first_link->length)
          : ImGui::InputFloat("Length", &first_link->length, 0, 0, "Varying");

  if (length_edited) {
    for (auto* link : links) {
      link->length = first_link->length;
    }
  }

  ImGui::ColorEdit3("Color", &edited_connection_.color.Value.x,
                    ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
  const auto color_edit_width =
      ImGui::GetItemRectSize().x + ImGui::GetStyle().ItemSpacing.x;

  ImGui::SameLine();
  ImGui::SetNextItemWidth(ImGui::CalcItemWidth() - color_edit_width);

  if (ImGui::Combo("Connection", &connection_name_index_,
                   connection_names_.data(),
                   static_cast<int>(connection_names_.size()))) {
  }

  ImGui::InputFloat("Attenuation/Length", &edited_connection_.drop_per_length);
  ImGui::InputFloat("Attenuation Added", &edited_connection_.drop_added);

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
void EditLinkPopup::Cancel(core::Diagram& diagram) const {
  for (const auto& link_copy : link_copies_) {
    auto& link = core::Diagram::FindLink(diagram, link_copy.id);
    link = link_copy;
  }
}

///
void EditLinkPopup::CopyLinksAndConnections(
    const std::vector<core::Link*>& links,
    const std::vector<core::Connection>& connections) {
  link_copies_.clear();
  link_copies_.reserve(links.size());

  std::transform(links.cbegin(), links.cend(), std::back_inserter(link_copies_),
                 [](const auto* link) { return *link; });

  connection_names_ = {"None"};
  connection_names_.reserve(connections.size() + 2);

  std::transform(
      connections.cbegin(), connections.cend(),
      std::back_inserter(connection_names_),
      [](const auto& connection) { return connection.name.c_str(); });

  connection_names_.emplace_back("Custom");
}
}  // namespace vh::ponc::draw