#include "draw_families_view.h"

#include "cpp_scope.h"
#include "draw_i_family_drawer.h"
#include "esc_app_state.h"
#include "imgui.h"

namespace esc::draw {
void FamiliesView::Draw(AppState& app_state) {
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

        for (const auto& family : app_state.app_.GetDiagram()->GetFamilies()) {
          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          ImGui::TextUnformatted(family->CreateDrawer()->GetLabel().c_str());
          ImGui::TableNextColumn();
          ImGui::Text("%d", 33);
        }
      }
    }
  }
}
}  // namespace esc::draw