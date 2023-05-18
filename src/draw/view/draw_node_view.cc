/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_node_view.h"

#include <algorithm>
#include <cstdint>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "core_i_family.h"
#include "coreui_family.h"
#include "draw_table_flags.h"
#include "imgui.h"
#include "imgui_node_editor.h"

namespace vh::ponc::draw {
namespace {
///
void DrawFamily(const coreui::Family& family, int count) {
  ImGui::TableNextRow();

  ImGui::TableNextColumn();
  ImGui::TextUnformatted(family.GetLabel().c_str());

  ImGui::TableNextColumn();
  ImGui::Text("%d", count);
}
}  // namespace

///
auto NodeView::GetLabel() const -> std::string { return "Node"; }

///
void NodeView::Draw(std::optional<const coreui::Node*> node,
                    const std::vector<coreui::FamilyGroup>& family_groups) {
  const auto content_scope = DrawContentScope();

  if (!IsOpened()) {
    return;
  }

  if (!node.has_value()) {
    ImGui::TextUnformatted("Select single Node.");
    ImGui::Separator();
    return;
  }

  ImGui::TextUnformatted((*node)->GetData().label.c_str());

  if (ImGui::BeginTable("Children", 2, kExpandingTableFlags)) {
    ImGui::TableSetupScrollFreeze(0, 1);
    ImGui::TableSetupColumn("Child Type");
    ImGui::TableSetupColumn("Count");
    ImGui::TableHeadersRow();

    for (const auto& family_group : family_groups) {
      for (const auto& family : family_group.families) {
        const auto family_id = family.GetFamily().GetId().Get();
        const auto& num_children_per_family =
            (*node)->GetTreeNode().num_children_per_family;

        if (num_children_per_family.contains(family_id)) {
          DrawFamily(family, num_children_per_family.at(family_id));
        }
      }
    }

    ImGui::EndTable();
  }
}
}  // namespace vh::ponc::draw