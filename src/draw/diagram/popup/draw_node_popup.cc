#include "draw_node_popup.h"

#include <string>
#include <vector>

#include "core_diagram.h"
#include "core_i_node.h"
#include "coreui_family.h"
#include "coreui_i_node_traits.h"
#include "draw_family_groups_menu.h"
#include "imgui.h"
#include "imgui_node_editor.h"

namespace esc::draw {
namespace {
///
auto GetSelectedNodes() {
  auto selected_nodes = std::vector<ne::NodeId>{};
  selected_nodes.resize(ne::GetSelectedObjectCount());

  const auto num_selected_nodes = ne::GetSelectedNodes(
      selected_nodes.data(), static_cast<int>(selected_nodes.size()));
  selected_nodes.resize(num_selected_nodes);

  return selected_nodes;
}

///
auto GetTitle(const std::vector<ne::NodeId>& nodes) {
  if (nodes.empty()) {
    return std::string{};
  }

  const auto num_nodes = nodes.size();

  if (num_nodes == 1) {
    return "Node #" + std::to_string(nodes.front().Get());
  }

  return "Nodes (" + std::to_string(nodes.size()) + ")";
}
}  // namespace

///
void NodePopup::Draw(coreui::Diagram& diagram) {
  const auto selected_nodes =
      IsOpened() ? GetSelectedNodes() : std::vector<ne::NodeId>{};
  const auto title = GetTitle(selected_nodes);
  const auto content_scope = DrawContentScope(title);

  if (!IsOpened()) {
    return;
  }

  if (ImGui::MenuItem("Delete")) {
    for (const auto node_id : selected_nodes) {
      diagram.DeleteNode(node_id);
    }
  }

  if (ImGui::MenuItem("Delete With Links")) {
    for (const auto node_id : selected_nodes) {
      diagram.DeleteNodeWithLinks(node_id);
    }
  }

  if (selected_nodes.size() > 1) {
    return;
  }

  const auto& node =
      core::Diagram::FindNode(diagram.GetDiagram(), selected_nodes.front());

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
}  // namespace esc::draw