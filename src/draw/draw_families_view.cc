#include "draw_families_view.h"

#include "cpp_scope.h"
#include "draw_i_family_drawer.h"
#include "esc_state.h"
#include "imgui.h"

namespace esc::draw {
void FamiliesView::Draw(State& state) {
  if (!IsVisible()) {
    return;
  }

  {
    const auto families_scope = cpp::Scope{[]() { ImGui::End(); }};

    if (ImGui::Begin("Node Families", &GetVisible())) {
      if (ImGui::BeginTable("3ways", 2)) {
        const auto table_scope = cpp::Scope{[]() { ImGui::EndTable(); }};

        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Number");
        ImGui::TableHeadersRow();

        for (const auto& family : state.app_.GetDiagram()->GetFamilies()) {
          ImGui::TableNextRow();
          ImGui::TableNextColumn();

          if (ImGui::TreeNode(family->CreateDrawer()->GetLabel().c_str())) {
            const auto tree_node_scope = cpp::Scope{[]() { ImGui::TreePop(); }};

            for (const auto& node : family->GetNodes()) {
              ImGui::Text("#%d", node->GetId().Get());
            }
          }

          ImGui::TableNextColumn();
          ImGui::Text("%d", family->GetNodes().size());
        }
      }
    }
  }
}
}  // namespace esc::draw