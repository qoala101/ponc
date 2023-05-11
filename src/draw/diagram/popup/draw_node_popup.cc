#include "draw_node_popup.h"

#include <string>
#include <vector>

#include "core_diagram.h"
#include "core_i_node.h"
#include "coreui_family.h"
#include "coreui_i_node_traits.h"
#include "coreui_native_facade.h"
#include "cpp_assert.h"
#include "draw_family_groups_menu.h"
#include "flow_algorithms.h"
#include "flow_tree_traversal.h"
#include "imgui.h"
#include "imgui_node_editor.h"

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
  ImGui::Separator();

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
void DrawNodeActions(const core::INode& node) {
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
}  // namespace

///
void NodePopup::Draw(coreui::Diagram& diagram) {
  const auto selected_nodes = IsOpened()
                                  ? coreui::NativeFacade::GetSelectedNodes()
                                  : std::vector<ne::NodeId>{};
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

  ImGui::Separator();

  if (ImGui::MenuItem("Tree Select")) {
    diagram.TreeSelect(selected_nodes);
  }

  if (ImGui::MenuItem("Tree Arrange")) {
    diagram.TreeArrange(selected_nodes);
  }

  if (selected_nodes.size() > 1) {
    return;
  }

  const auto& node =
      core::Diagram::FindNode(diagram.GetDiagram(), selected_nodes.front());

  DrawReplaceActions(diagram, node);
  DrawNodeActions(node);
}
}  // namespace vh::ponc::draw