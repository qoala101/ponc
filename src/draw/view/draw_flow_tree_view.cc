#include "draw_flow_tree_view.h"

#include <iostream>
#include <numeric>
#include <string>

#include "core_i_family.h"
#include "core_i_node.h"
#include "coreui_diagram.h"
#include "coreui_i_family_traits.h"
#include "coreui_i_node_traits.h"
#include "coreui_project.h"
#include "cpp_safe_ptr.h"
#include "cpp_scope.h"
#include "draw_id_label.h"
#include "draw_native_facade.h"
#include "flow_algorithms.h"
#include "flow_tree.h"
#include "flow_tree_traversal.h"
#include "imgui.h"
#include "imgui_node_editor.h"

namespace esc::draw {
namespace {
auto IsChildOf(const core::INode& node, const core::IFamily& family) {
  return node.GetFamilyId() == family.GetId();
}

void DrawNode(
    const coreui::Diagram& diagram, const flow::TreeNode& tree_node,
    const std::unordered_map<const flow::TreeNode*,
                             std::vector<std::pair<const core::IFamily*, int>>>&
        child_count_per_family_per_tree_node) {
  ImGui::TableNextRow();
  ImGui::TableNextColumn();

  const auto node_id = tree_node.node->GetId();
  const auto& node = coreui::Diagram::FindNode(diagram, node_id);

  const auto has_children = !tree_node.child_nodes.empty();
  const auto tree_node_flags =
      has_children ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_Leaf;

  const auto tree_node_is_open =
      ImGui::TreeNodeEx(IdLabel(node_id).c_str(), tree_node_flags);
  ImGui::SameLine();

  const auto& node_data = node.GetData();
  auto is_selected = ne::IsNodeSelected(node_id);

  if (ImGui::Selectable(node_data.label.c_str(), &is_selected)) {
    const auto ctrl_pressed = ImGui::GetIO().KeyCtrl;

    if (ctrl_pressed) {
      if (is_selected) {
        ne::SelectNode(node_id, true);
      } else {
        ne::DeselectNode(node_id);
      }
    } else {
      ne::SelectNode(node_id, false);
    }

    ne::NavigateToSelection();
  }

  ImGui::TableNextColumn();

  if (const auto& input_flow = node_data.flow.input_flow) {
    ImGui::TextColored(input_flow->color, "%.3f", input_flow->value);
  }

  ImGui::TableNextColumn();
  ImGui::BeginHorizontal(node_id.AsPointer());

  const auto& output_flows = node_data.flow.output_flows;
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

  const auto& child_cout_per_family =
      child_count_per_family_per_tree_node.at(&tree_node);

  auto index = 0;

  for (const auto& family_group : diagram.GetFamilyGroups()) {
    for (const auto& family : family_group.families) {
      ImGui::TableNextColumn();

      auto child_count = child_cout_per_family[index++].second;

      if (IsChildOf(*tree_node.node, family.GetFamily())) {
        if (child_count == 1) {
          ImGui::TextUnformatted("-");
        } else {
          ImGui::Text("%d", child_count - 1);
        }
      } else {
        if (child_count > 0) {
          ImGui::Text("%d", child_count);
        }
      }
    }
  }

  if (tree_node_is_open) {
    if (has_children) {
      for (const auto& child_node : tree_node.child_nodes) {
        DrawNode(diagram, child_node.second,
                 child_count_per_family_per_tree_node);
      }
    }

    ImGui::TreePop();
  }
}
}  // namespace

auto FlowTreeView::GetLabel() const -> std::string { return "Flow Tree"; }

void FlowTreeView::Draw(const coreui::Diagram& diagram) {
  const auto content_scope = DrawContentScope();

  if (!IsOpened()) {
    return;
  }

  const auto& family_groups = diagram.GetFamilyGroups();
  const auto num_families =
      std::accumulate(family_groups.begin(), family_groups.end(), 0,
                      [](auto sum, const auto& family_group) {
                        return sum + family_group.families.size();
                      });

  // NOLINTBEGIN(*-signed-bitwise)
  const auto table_flags =
      ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
      ImGuiTableFlags_Hideable | ImGuiTableFlags_ContextMenuInBody |
      ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersOuter |
      ImGuiTableFlags_ScrollY;
  // NOLINTEND(*-signed-bitwise)

  if (ImGui::BeginTable("Flow Tree", 3 + num_families, table_flags)) {
    const auto table_scope = cpp::Scope{[]() { ImGui::EndTable(); }};

    ImGui::TableSetupColumn("Name");
    ImGui::TableSetupColumn("Input");
    ImGui::TableSetupColumn("Output");

    for (const auto& family_group : family_groups) {
      for (const auto& family : family_group.families) {
        ImGui::TableSetupColumn((family.GetLabel() + " #").c_str());
      }
    }

    ImGui::TableHeadersRow();

    auto safe_owner = cpp::SafeOwner{};
    const auto flow_tree =
        flow::BuildFlowTree(diagram.GetDiagram(), safe_owner);
    const auto node_flows = flow::CalculateNodeFlows(flow_tree);

    for (const auto& root_node : flow_tree.root_nodes) {
      auto child_count_per_family_per_tree_node = std::unordered_map<
          const flow::TreeNode*,
          std::vector<std::pair<const core::IFamily*, int>>>{};

      flow::TraverseDepthFirst(
          root_node, [](const auto&) {},
          [&family_groups, &child_count_per_family_per_tree_node](
              const flow::TreeNode& tree_node) {
            auto child_count_per_family =
                std::vector<std::pair<const core::IFamily*, int>>{};

            for (const auto& family_group : family_groups) {
              for (const auto& family : family_group.families) {
                const auto value =
                    IsChildOf(*tree_node.node, family.GetFamily()) ? 1 : 0;
                child_count_per_family.emplace_back(&family.GetFamily(), value);
              }
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

      DrawNode(diagram, root_node, child_count_per_family_per_tree_node);
    }
  }
}
}  // namespace esc::draw