#include "draw_flow_tree_view.h"

#include <iostream>
#include <string>

#include "core_i_family.h"
#include "core_tree.h"
#include "cpp_scope.h"
#include "draw_families_view.h"
#include "draw_i_family_drawer.h"
#include "draw_i_node_drawer.h"
#include "esc_state.h"
#include "imgui.h"
#include "imgui_node_editor.h"

namespace esc::draw {
namespace {
void DisplayNode(
    State& state, const core::TreeNode& tree_node,
    std::unordered_map<core::IFamily*, int>& child_count_per_family) {
  ImGui::TableNextRow();

  ImGui::TableNextColumn();
  const auto drawer = tree_node.node->CreateDrawer(state);

  const auto node_id = tree_node.node->GetId();
  const auto has_children = !tree_node.child_nodes.empty();
  const auto draw_flags =
      has_children ? (ImGuiTreeNodeFlags_DefaultOpen)
                   : ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet;

  const auto node_is_open = ImGui::TreeNodeEx(
      std::string{"##" + std::to_string(static_cast<uintptr_t>(node_id.Get()))}
          .c_str(),
      draw_flags);

  ImGui::SameLine();

  std::vector<ne::NodeId> selectedNodes;
  std::vector<ne::LinkId> selectedLinks;
  selectedNodes.resize(ne::GetSelectedObjectCount());
  selectedLinks.resize(ne::GetSelectedObjectCount());

  int nodeCount = ne::GetSelectedNodes(selectedNodes.data(),
                                       static_cast<int>(selectedNodes.size()));
  int linkCount = ne::GetSelectedLinks(selectedLinks.data(),
                                       static_cast<int>(selectedLinks.size()));

  selectedNodes.resize(nodeCount);
  selectedLinks.resize(linkCount);

  bool isSelected = std::find(selectedNodes.begin(), selectedNodes.end(),
                              node_id) != selectedNodes.end();
  if (ImGui::Selectable(
          std::string{drawer->GetLabel() + "##" +
                      std::to_string(static_cast<uintptr_t>(node_id.Get()))}
              .c_str(),
          &isSelected)) {
    auto& io = ImGui::GetIO();

    if (io.KeyCtrl) {
      if (isSelected)
        ne::SelectNode(node_id, true);
      else
        ne::DeselectNode(node_id);
    } else
      ne::SelectNode(node_id, false);

    ne::NavigateToSelection();
  }

  ImGui::TableNextColumn();
  const auto flow = state.flow_calculator_.GetCalculatedFlow(*tree_node.node);

  if (flow.input_pin_flow.has_value()) {
    ImGui::Text("%.3f", flow.input_pin_flow->second);
  }

  if (node_is_open) {
    if (has_children) {
      for (const auto& child_node : tree_node.child_nodes) {
        DisplayNode(state, child_node.second, child_count_per_family);

        for (const auto& child_count : child_count_per_family) {
          ImGui::TableNextColumn();
          ImGui::Text("%d", child_count.second);
        }
      }
    }

    ImGui::TreePop();
  }
}
}  // namespace

void FlowTreeView::Draw(State& state) {
  if (!IsVisible()) {
    return;
  }

  {
    const auto window_scope = cpp::Scope{[]() { ImGui::End(); }};

    if (ImGui::Begin("Flow Tree", &GetVisible())) {
      const auto& families = state.app_.GetDiagram().GetFamilies();
      auto child_count_per_family = std::unordered_map<core::IFamily*, int>{};

      for (const auto& family : families) {
        child_count_per_family.emplace(family.get(), 0);
      }

      ImGui::SameLine();

      const auto table_flags =
          ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
          ImGuiTableFlags_Hideable | ImGuiTableFlags_ContextMenuInBody |
          ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersOuter |
          ImGuiTableFlags_ScrollY;

      if (ImGui::BeginTable("Flow Tree", 2 + families.size(), table_flags)) {
        const auto table_scope = cpp::Scope{[]() { ImGui::EndTable(); }};

        ImGui::TableSetupColumn("Item", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Input Value",
                                ImGuiTableColumnFlags_WidthFixed);

        for (const auto& family : families) {
          ImGui::TableSetupColumn(family->CreateDrawer()->GetLabel().c_str(),
                                  ImGuiTableColumnFlags_WidthFixed);
        }

        ImGui::TableHeadersRow();

        for (const auto& root_node :
             state.flow_calculator_.GetFlowTree().root_nodes) {
          DisplayNode(state, root_node, child_count_per_family);
        }
      }
    }
  }
}
}  // namespace esc::draw