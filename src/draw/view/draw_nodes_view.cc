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
#include "draw_tree_node.h"
#include "imgui.h"
#include "imgui_node_editor.h"

namespace esc::draw {
namespace {
///
void DrawSelectableName(
    const coreui::Family& family,
    const std::unordered_set<uintptr_t>& selected_node_ids) {
  const auto& nodes = family.GetNodes();
  const auto label =
      family.GetLabel() + " (" + std::to_string(nodes.size()) + ")";

  auto item_is_selected = std::all_of(
      nodes.begin(), nodes.end(), [&selected_node_ids](const auto& node) {
        return selected_node_ids.contains(node->GetNode().GetId().Get());
      });

  if (ImGui::Selectable(label.c_str(), &item_is_selected)) {
    const auto ctrl_pressed = ImGui::GetIO().KeyCtrl;

    if (ctrl_pressed) {
      if (item_is_selected) {
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

  const auto item_is_open =
      ImGui::TreeNodeEx(IdLabel(family.GetFamily().GetId()).c_str(),
                        ImGuiTreeNodeFlags_DefaultOpen);
  ImGui::SameLine();
  DrawSelectableName(family, selected_node_ids);

  if (!item_is_open) {
    return;
  }

  for (const auto& node : nodes) {
    DrawTreeNode(node->GetTreeNode(), false);
  }

  ImGui::TreePop();
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
                   [](const auto node_id) { return node_id.Get(); });

    for (const auto& family_group : diagram.GetFamilyGroups()) {
      for (const auto& family : family_group.families) {
        DrawFamily(family, selected_node_ids);
      }
    }

    ImGui::EndTable();
  }
}
}  // namespace esc::draw