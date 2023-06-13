/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_node_popup.h"

#include <imgui.h>
#include <imgui_node_editor.h>

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "core_diagram.h"
#include "core_i_node.h"
#include "core_tag.h"
#include "coreui_family.h"
#include "coreui_i_node_traits.h"
#include "coreui_native_facade.h"
#include "cpp_callbacks.h"
#include "draw_disable_if.h"
#include "draw_family_groups_menu.h"

namespace vh::ponc::draw {
namespace {
///
auto GetTitle(const std::vector<ne::NodeId>& nodes) {
  if (nodes.empty()) {
    return std::string{};
  }

  if (nodes.size() == 1) {
    return "Node #" + std::to_string(nodes.front().Get());
  }

  return "Nodes (" + std::to_string(nodes.size()) + ")";
}

///
void DrawReplaceActions(coreui::Diagram& diagram, const core::INode& node) {
  if (ImGui::BeginMenu("Replace With")) {
    FamilyGroupsMenu::Draw(
        diagram.GetFamilyGroups(),
        {.is_family_enabled =
             [&diagram, &node](const auto& family) {
               const auto sample_node = family.CreateSampleNode();
               return diagram.CanReplaceNode(node, *sample_node);
             },
         .family_selected =
             [&diagram, &node](const auto& family) {
               diagram.ReplaceNode(node, family.CreateNode());
             }});

    ImGui::EndMenu();
  }
}

///
void DrawNodeActions(core::INode& node) {
  const auto node_traits = node.CreateUiTraits();
  const auto node_action_names = node_traits->GetActionNames();

  if (node_action_names.empty()) {
    return;
  }

  ImGui::Separator();

  for (const auto& action_name : node_action_names) {
    if (ImGui::MenuItem(action_name.c_str())) {
      node_traits->ExecuteAction(action_name);
    }
  }
}

///
auto FindNodes(const core::Diagram& diagram,
               const std::vector<ne::NodeId>& node_ids) {
  auto nodes = std::vector<core::INode*>{};
  nodes.reserve(node_ids.size());

  std::transform(node_ids.cbegin(), node_ids.cend(), std::back_inserter(nodes),
                 [&diagram](const auto node_id) {
                   return &core::Diagram::FindNode(diagram, node_id);
                 });

  return nodes;
}
}  // namespace

///
void NodePopup::Draw(coreui::Diagram& diagram, coreui::Tags& tags) {
  const auto selected_node_ids = IsOpened()
                                     ? coreui::NativeFacade::GetSelectedNodes()
                                     : std::vector<ne::NodeId>{};
  const auto title = GetTitle(selected_node_ids);
  const auto content_scope = DrawContentScope(title);

  if (!IsOpened()) {
    return;
  }

  if (ImGui::MenuItem("Delete")) {
    for (const auto node_id : selected_node_ids) {
      diagram.DeleteNode(node_id);
    }
  }

  if (ImGui::MenuItem("Delete With Links")) {
    for (const auto node_id : selected_node_ids) {
      diagram.DeleteNodeWithLinks(node_id);
    }
  }

  ImGui::Separator();

  const auto selected_nodes =
      FindNodes(diagram.GetDiagram(), selected_node_ids);

  DrawTagActions(tags, selected_nodes);
  ImGui::Separator();

  if (ImGui::MenuItem("Tree Select")) {
    diagram.TreeSelect(selected_node_ids);
  }

  if (ImGui::MenuItem("Tree Arrange")) {
    diagram.TreeArrange(selected_node_ids);
  }

  if (selected_node_ids.size() > 1) {
    return;
  }

  auto* node = selected_nodes.front();

  ImGui::Separator();
  DrawReplaceActions(diagram, *node);
  DrawNodeActions(*node);
}

///
void NodePopup::OnOpen() {
  tag_name_buffer_.Clear();
  tag_checks_.clear();
}

///
void NodePopup::DrawTagActions(
    coreui::Tags& tags, const std::vector<core::INode*>& selected_nodes) {
}
}  // namespace vh::ponc::draw