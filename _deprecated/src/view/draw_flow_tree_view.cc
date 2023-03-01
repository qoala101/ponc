#include "draw_flow_tree_view.h"

#include <iostream>
#include <string>

#include "core_i_family.h"
#include "core_i_node.h"
#include "coreui_i_family_traits.h"
#include "coreui_i_node_traits.h"
#include "coreui_project.h"
#include "cpp_scope.h"
#include "draw_families_view.h"
#include "flow_tree.h"
#include "imgui.h"
#include "imgui_node_editor.h"

namespace esc::draw {
namespace {
void DisplayNode(
    coreui::Frame& frame, const flow::TreeNode& tree_node,
    const std::unordered_map<const flow::TreeNode*,
                             std::vector<std::pair<const core::IFamily*, int>>>&
        child_count_per_family_per_tree_node) {
  ImGui::TableNextRow();

  ImGui::TableNextColumn();
  const auto drawer = tree_node.node->CreateUiTraits(state.ToStateNoQueue());

  const auto node_id = tree_node.node->GetId();
  const auto has_children = !tree_node.child_nodes.empty();
  const auto draw_flags =
      has_children ? (ImGuiTreeNodeFlags_DefaultOpen) : ImGuiTreeNodeFlags_Leaf;

  const auto node_is_open = ImGui::TreeNodeEx(
      std::string{"##" + std::to_string(static_cast<uintptr_t>(node_id.Get()))}
          .c_str(),
      draw_flags);

  ImGui::SameLine();

  const auto selectedNodes = state.core_state->diagram_.GetSelectedNodeIds();

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
  const auto flow =
      state.core_state->flow_calculator_.GetCalculatedFlow(*tree_node.node);

  if (flow.input_pin_flow.has_value()) {
    ImGui::TextColored(state.core_state->flow_colors_.GetColorForFlowValue(
                           flow.input_pin_flow->second),
                       "%.3f", flow.input_pin_flow->second);
  }

  const auto& child_cout_per_family =
      child_count_per_family_per_tree_node.at(&tree_node);

  auto index = 0;

  for (const auto& family : state.core_state->diagram_.GetFamilies()) {
    ImGui::TableNextColumn();

    auto child_count = child_cout_per_family[index++].second;

    if (IsChildOf(*tree_node.node, *family)) {
      if (child_count == 1) {
        ImGui::TextUnformatted("*");
      } else {
        ImGui::Text("%d", child_count - 1);
      }
    } else {
      if (child_count > 0) {
        ImGui::Text("%d", child_count);
      }
    }
  }

  if (node_is_open) {
    if (has_children) {
      for (const auto& child_node : tree_node.child_nodes) {
        DisplayNode(state, child_node.second,
                    child_count_per_family_per_tree_node);
      }
    }

    ImGui::TreePop();
  }
}
}  // namespace

auto FlowTreeView::GetLabel() const -> std::string { return "Flow Tree"; }

void FlowTreeView::Draw(coreui::Frame& frame) {
  if (!IsVisible()) {
    return;
  }

  {
    const auto window_scope = cpp::Scope{[]() { ImGui::End(); }};

    if (ImGui::Begin("Flow Tree", &GetVisible())) {
      const auto& families = state.core_state->diagram_.GetFamilies();
      const auto table_flags =
          ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
          ImGuiTableFlags_Hideable | ImGuiTableFlags_ContextMenuInBody |
          ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersOuter |
          ImGuiTableFlags_ScrollY;

      if (ImGui::BeginTable("Flow Tree", 2 + families.size(), table_flags)) {
        const auto table_scope = cpp::Scope{[]() { ImGui::EndTable(); }};

        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 300);
        ImGui::TableSetupColumn("Input");

        for (const auto& family : families) {
          ImGui::TableSetupColumn(
              std::string{family->CreateUiTraits()->GetLabel() + " #"}.c_str());
        }

        ImGui::TableHeadersRow();

        for (const auto& root_node :
             state.core_state->flow_calculator_.GetFlowTree().root_nodes) {
          auto child_count_per_family_per_tree_node = std::unordered_map<
              const flow::TreeNode*,
              std::vector<std::pair<const core::IFamily*, int>>>{};

          TraverseDepthFirst(
              root_node, {},
              [&families, &child_count_per_family_per_tree_node](
                  const flow::TreeNode& tree_node) {
                auto child_count_per_family =
                    std::vector<std::pair<const core::IFamily*, int>>{};

                for (const auto& family : families) {
                  const auto value =
                      IsChildOf(*tree_node.node, *family) ? 1 : 0;
                  child_count_per_family.emplace_back(&*family, value);
                }

                for (const auto& child : tree_node.child_nodes) {
                  const auto& child_family_counts =
                      child_count_per_family_per_tree_node.at(&child.second);

                  auto index = 0;

                  for (const auto& child_family_count : child_family_counts) {
                    child_count_per_family[index++].second +=
                        child_family_count.second;
                  }
                }

                child_count_per_family_per_tree_node.emplace(
                    &tree_node, std::move(child_count_per_family));
              });

          DisplayNode(state, root_node, child_count_per_family_per_tree_node);
        }
      }
    }
  }
}
}  // namespace esc::draw