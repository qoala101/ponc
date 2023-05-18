/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_flow_tree_view.h"

#include <imgui.h>

#include <numeric>
#include <string>

#include "coreui_flow_tree_node.h"
#include "draw_table_flags.h"
#include "draw_tree_node.h"

namespace vh::ponc::draw {
///
auto FlowTreeView::GetLabel() const -> std::string { return "Flow Tree"; }

///
void FlowTreeView::Draw(const std::vector<coreui::TreeNode>& node_trees) {
  const auto content_scope = DrawContentScope();

  if (!IsOpened()) {
    return;
  }

  if (ImGui::BeginTable("Flow Tree", 3, kExpandingTableFlags)) {
    ImGui::TableSetupScrollFreeze(0, 1);
    ImGui::TableSetupColumn("Node");
    ImGui::TableSetupColumn("Input");
    ImGui::TableSetupColumn("Output");
    ImGui::TableHeadersRow();

    for (const auto& root_node : node_trees) {
      DrawTreeNode(root_node);
    }

    ImGui::EndTable();
  }
}
}  // namespace vh::ponc::draw