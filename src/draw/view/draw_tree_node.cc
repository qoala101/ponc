#include "draw_tree_node.h"

#include <iostream>
#include <string_view>

#include "coreui_node.h"
#include "draw_id_label.h"
#include "imgui_node_editor.h"

namespace vh::ponc::draw {
namespace {
///
void DrawSelectableName(ne::NodeId node_id, std::string_view node_label) {
  auto selected = ne::IsNodeSelected(node_id);

  if (ImGui::Selectable(node_label.data(), &selected)) {
    const auto ctrl_pressed = ImGui::GetIO().KeyCtrl;

    if (ctrl_pressed) {
      if (selected) {
        ne::SelectNode(node_id, true);
      } else {
        ne::DeselectNode(node_id);
      }
    } else {
      ne::SelectNode(node_id, false);
    }

    ne::NavigateToSelection();
  }
}

///
void DrawInputFlow(const std::optional<coreui::FlowValue>& input_flow) {
  if (input_flow.has_value()) {
    ImGui::TextColored(input_flow->color, "%.3f", input_flow->value);
  }
}

///
void DrawOutputFlows(ne::NodeId node_id,
                     const std::vector<coreui::FlowValue>& output_flows) {
  ImGui::BeginHorizontal(node_id.AsPointer());

  const auto output_flows_end = output_flows.end();
  auto output_flow = output_flows.begin();

  if (output_flow != output_flows_end) {
    ImGui::TextColored(output_flow->color, "%.3f", output_flow->value);
    ++output_flow;
  }

  for (; output_flow != output_flows_end; ++output_flow) {
    ImGui::TextUnformatted("/");
    ImGui::TextColored(output_flow->color, "%.3f", output_flow->value);
  }

  ImGui::EndHorizontal();
}
}  // namespace

///
// NOLINTNEXTLINE(*-no-recursion)
void DrawTreeNode(const coreui::TreeNode& tree_node, bool draw_children) {
  ImGui::TableNextRow();
  ImGui::TableNextColumn();

  const auto node_id = tree_node.node->GetNode().GetId();
  const auto& node_data = tree_node.node->GetData();
  const auto& child_nodes = tree_node.child_nodes;
  const auto item_flags = (draw_children && !child_nodes.empty())
                              ? ImGuiTreeNodeFlags_DefaultOpen
                              : ImGuiTreeNodeFlags_Leaf;

  const auto item_is_open =
      ImGui::TreeNodeEx(IdLabel(node_id).c_str(), item_flags);
  ImGui::SameLine();
  DrawSelectableName(node_id, node_data.label);

  ImGui::TableNextColumn();
  DrawInputFlow(node_data.flow.input_flow);

  ImGui::TableNextColumn();
  DrawOutputFlows(node_id, node_data.flow.output_flows);

  if (!item_is_open) {
    return;
  }

  if (draw_children) {
    for (const auto& child_node : child_nodes) {
      DrawTreeNode(child_node, draw_children);
    }
  }

  ImGui::TreePop();
}
}  // namespace vh::ponc::draw