#include "draw_nodes_view.h"

#include <algorithm>
#include <cstdint>
#include <set>
#include <string>
#include <unordered_set>

#include "core_i_family.h"
#include "coreui_family.h"
#include "draw_id_label.h"
#include "draw_native_facade.h"
#include "imgui.h"
#include "imgui_node_editor.h"

namespace esc::draw {
namespace {
///
void DrawNode(const coreui::Node& node) {
  ImGui::TableNextRow();
  ImGui::TableNextColumn();

  const auto node_id = node.GetNode().GetId();

  ImGui::TreeNodeEx(IdLabel(node_id).c_str(), ImGuiTreeNodeFlags_Leaf);
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
  ImGui::TreePop();
}

///
void DrawFamily(const coreui::Family& family,
                const std::unordered_set<uintptr_t>& selected_node_ids) {
  const auto& nodes = family.GetNodes();

  if (nodes.empty()) {
    return;
  }

  ImGui::TableNextRow();
  ImGui::TableNextColumn();

  const auto tree_node_is_open =
      ImGui::TreeNodeEx(IdLabel(family.GetFamily().GetId()).c_str(),
                        ImGuiTreeNodeFlags_DefaultOpen);
  ImGui::SameLine();

  const auto label =
      family.GetLabel() + " (" + std::to_string(nodes.size()) + ")";

  auto is_selected = std::all_of(
      nodes.begin(), nodes.end(), [&selected_node_ids](const auto& node) {
        return selected_node_ids.contains(node->GetNode().GetId().Get());
      });

  if (ImGui::Selectable(label.c_str(), &is_selected)) {
    const auto ctrl_pressed = ImGui::GetIO().KeyCtrl;

    if (ctrl_pressed) {
      if (is_selected) {
        for (const auto& node : nodes) {
          ne::SelectNode(node->GetNode().GetId(), true);
        }
      } else {
        for (const auto& node : nodes) {
          ne::DeselectNode(node->GetNode().GetId());
        }
      }
    } else {
      ne::ClearSelection();

      for (const auto& node : nodes) {
        ne::SelectNode(node->GetNode().GetId(), true);
      }
    }

    ne::NavigateToSelection();
  }

  if (tree_node_is_open) {
    for (const auto& node : nodes) {
      DrawNode(*node);
    }

    ImGui::TreePop();
  }
}
}  // namespace

///
auto NodesView::GetLabel() const -> std::string { return "Nodes"; }

///
void NodesView::Draw(const coreui::Diagram& diagram) {
  const auto content_scope = DrawContentScope();

  if (!IsOpened()) {
    return;
  }

  // NOLINTBEGIN(*-signed-bitwise)
  const auto table_flags =
      ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
      ImGuiTableFlags_Hideable | ImGuiTableFlags_ContextMenuInBody |
      ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersOuter |
      ImGuiTableFlags_ScrollY;
  // NOLINTEND(*-signed-bitwise)

  if (ImGui::BeginTable("Nodes", 3, table_flags)) {
    ImGui::TableSetupColumn("Name");
    ImGui::TableSetupColumn("Input");
    ImGui::TableSetupColumn("Output");
    ImGui::TableHeadersRow();

    const auto selected_nodes = NativeFacade::GetSelectedNodes();

    auto selected_node_ids = std::unordered_set<uintptr_t>{};
    std::transform(selected_nodes.begin(), selected_nodes.end(),
                   std::inserter(selected_node_ids, selected_node_ids.begin()),
                   [](auto node_id) { return node_id.Get(); });

    for (const auto& family_group : diagram.GetFamilyGroups()) {
      for (const auto& family : family_group.families) {
        DrawFamily(family, selected_node_ids);
      }
    }

    ImGui::EndTable();
  }
}
}  // namespace esc::draw