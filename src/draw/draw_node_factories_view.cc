#include "draw_node_factories_view.h"

#include "cpp_scope.h"
#include "draw_i_node_factory_drawer.h"
#include "esc_app_state.h"
#include "imgui.h"

namespace esc::draw {
void NodeFactoriesView::Draw(AppState& app_state) {
  if (!IsVisible()) {
    return;
  }

  {
    const auto node_factories_scope = cpp::Scope{[]() { ImGui::End(); }};

    if (ImGui::Begin("Node Factories", &GetVisible())) {
      if (ImGui::BeginTable("3ways", 2)) {
        const auto table_scope = cpp::Scope{[]() { ImGui::EndTable(); }};

        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Number");
        ImGui::TableHeadersRow();

        for (const auto& node_factory :
             app_state.app_.GetDiagram()->GetNodeFactories()) {
          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          ImGui::TextUnformatted(
              node_factory->CreateDrawer()->GetLabel().c_str());
          ImGui::TableNextColumn();
          ImGui::Text("%d", 33);
        }
      }
    }
  }
}
}  // namespace esc::draw