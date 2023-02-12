#include "draw_flow_tree_view.h"

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
void DisplayNode(State& state, const core::TreeNode& tree_node) {
  ImGui::TableNextRow();
  ImGui::TableNextColumn();

  const auto is_folder = !tree_node.child_nodes.empty();
  const auto drawer = tree_node.node->CreateDrawer(state);

  if (is_folder) {
    auto open = ImGui::TreeNodeEx(drawer->GetLabel().c_str(),
                                  ImGuiTreeNodeFlags_SpanFullWidth);
    ImGui::TableNextColumn();
    ImGui::Text("%d", 33);
    if (open) {
      for (const auto& child_node : tree_node.child_nodes) {
        DisplayNode(state, child_node);
      }
      ImGui::TreePop();
    }
  } else {
    ImGui::TreeNodeEx(drawer->GetLabel().c_str(),
                      ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet |
                          ImGuiTreeNodeFlags_NoTreePushOnOpen |
                          ImGuiTreeNodeFlags_SpanFullWidth);
    ImGui::TableNextColumn();
    ImGui::Text("%d", 33);
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
      if (ImGui::BeginTable("3ways", 2)) {
        const auto table_scope = cpp::Scope{[]() { ImGui::EndTable(); }};

        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Size");
        ImGui::TableHeadersRow();

        for (const auto& root_node : state.flow_calculator_.GetFlowTree().root_nodes) {
          DisplayNode(state, root_node);
        }
      }
    }
  }
}
}  // namespace esc::draw