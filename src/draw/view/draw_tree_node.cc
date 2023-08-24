/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_tree_node.h"

#include <imgui.h>
#include <imgui_node_editor.h>

#include <optional>
#include <string>
#include <string_view>

#include "core_i_node.h"
#include "coreui_native_facade.h"
#include "coreui_node.h"
#include "cpp_safe_ptr.h"

namespace vh::ponc::draw {
namespace {
///
void DrawSelectableName(ne::NodeId node_id, std::string_view node_label) {
  auto selected = ne::IsNodeSelected(node_id);

  if (ImGui::Selectable(node_label.data(), &selected)) {
    const auto ctrl_pressed = ImGui::GetIO().KeyCtrl;

    if (ctrl_pressed) {
      if (selected) {
        coreui::NativeFacade::SelectNode(node_id, true);
      } else {
        ne::DeselectNode(node_id);
      }
    } else {
      coreui::NativeFacade::SelectNode(node_id, false);
    }

    ne::NavigateToSelection();
  }
}
}  // namespace

///
void DrawInputFlow(const coreui::TreeNode& tree_node) {
  if (const auto& input_flow = tree_node.node->GetData().flow.input_flow) {
    ImGui::TextColored(input_flow->color, "%.2f", input_flow->value);
  }
}

///
void DrawOutputFlows(const coreui::TreeNode& tree_node) {
  // ImGui::BeginHorizontal(tree_node.node->GetNode().GetId().AsPointer());

  const auto& output_flows = tree_node.node->GetData().flow.output_flows;
  auto output_flow = output_flows.cbegin();

  if (output_flow != output_flows.cend()) {
    ImGui::TextColored(output_flow->color, "%.2f", output_flow->value);
    ++output_flow;
  }

  for (; output_flow != output_flows.cend(); ++output_flow) {
    ImGui::TextUnformatted("/");
    ImGui::TextColored(output_flow->color, "%.2f", output_flow->value);
  }

  // ImGui::EndHorizontal();
}

///
// NOLINTNEXTLINE(*-no-recursion)
void DrawTreeNode(
    const coreui::TreeNode& tree_node, bool draw_children, bool selectable,
    const std::vector<std::function<void(const coreui::TreeNode&)>>&
        draw_columns) {
  const auto node_id = tree_node.node->GetNode().GetId();

  ImGui::PushID(node_id.AsPointer());
  ImGui::TableNextRow();
  ImGui::TableNextColumn();

  const auto& node_data = tree_node.node->GetData();
  const auto& child_nodes = tree_node.child_nodes;
  const auto item_flags = (draw_children && !child_nodes.empty())
                              ? ImGuiTreeNodeFlags_DefaultOpen
                              : ImGuiTreeNodeFlags_Leaf;

  auto item_is_open = false;

  if (selectable) {
    item_is_open = ImGui::TreeNodeEx("", item_flags);
    ImGui::SameLine();
    DrawSelectableName(node_id, node_data.label);
  } else {
    item_is_open = ImGui::TreeNodeEx(
        node_data.label.c_str(), item_flags | ImGuiTreeNodeFlags_OpenOnArrow);
  }

  for (const auto& draw_column : draw_columns) {
    ImGui::TableNextColumn();
    draw_column(tree_node);
  }

  if (item_is_open) {
    if (draw_children) {
      for (const auto& child_node : child_nodes) {
        DrawTreeNode(child_node, draw_children, selectable, draw_columns);
      }
    }

    ImGui::TreePop();
  }

  ImGui::PopID();
}
}  // namespace vh::ponc::draw