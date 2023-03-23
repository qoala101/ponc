#include "draw_flow_tree_view.h"

#include <imgui.h>

#include <numeric>
#include <string>

#include "coreui_flow_tree.h"
#include "draw_tree_node.h"

namespace vh::ponc::draw {
///
auto FlowTreeView::GetLabel() const -> std::string { return "Flow Tree"; }

///
void FlowTreeView::Draw(const coreui::FlowTree &flow_tree) {
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

  if (ImGui::BeginTable("Flow Tree", 3, table_flags)) {
    ImGui::TableSetupColumn("Node");
    ImGui::TableSetupColumn("Input");
    ImGui::TableSetupColumn("Output");
    ImGui::TableHeadersRow();

    for (const auto &root_node : flow_tree.root_nodes) {
      DrawTreeNode(root_node);
    }

    ImGui::EndTable();
  }
}
}  // namespace vh::ponc::draw