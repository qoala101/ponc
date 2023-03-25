#include "draw_diagrams_view.h"

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <set>
#include <string>
#include <unordered_set>

#include "core_diagram.h"
#include "cpp_scope.h"
#include "cpp_scope_function.h"
#include "imgui.h"
#include "imgui_node_editor.h"

namespace vh::ponc::draw {
namespace {
///
auto DisableIf(bool condition) {
  if (!condition) {
    return cpp::ScopeFunction{[]() {}};
  }

  ImGui::PushDisabled();
  return cpp::ScopeFunction{[]() { ImGui::PopDisabled(); }};
}
}  // namespace

///
auto DiagramsView::GetLabel() const -> std::string { return "Diagrams"; }

///
void DiagramsView::Draw(coreui::Project& project) {
  const auto content_scope = DrawContentScope();

  if (!IsOpened()) {
    return;
  }

  const auto& diagrams = project.GetProject().GetDiagrams();
  auto delete_selected_diagram = false;

  ImGui::BeginHorizontal("Controls");

  {
    const auto disable_scope = DisableIf(diagrams.size() <= 1);

    if (ImGui::Button("Delete")) {
      delete_selected_diagram = true;
    }
  }

  if (ImGui::Button("Create")) {
    project.AddDiagram(core::Diagram{});
  }

  if (ImGui::Button("Clone")) {
  }

  ImGui::EndHorizontal();

  // NOLINTBEGIN(*-signed-bitwise)
  const auto table_flags =
      ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
      ImGuiTableFlags_Hideable | ImGuiTableFlags_ContextMenuInBody |
      ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersOuter |
      ImGuiTableFlags_ScrollY;
  // NOLINTEND(*-signed-bitwise)

  if (ImGui::BeginTable("Diagrams", 1, table_flags)) {
    auto index = 0;

    for (const auto& diagram : project.GetProject().GetDiagrams()) {
      ImGui::TableNextRow();
      ImGui::TableNextColumn();

      const auto item_is_selected =
          &diagram == &project.GetDiagram().GetDiagram();

      if (ImGui::Selectable(("Diagram " + std::to_string(index)).c_str(),
                            item_is_selected) &&
          !item_is_selected) {
        project.SetDiagram(index).Then(([]() { ne::NavigateToContent(); }));
      }

      if (item_is_selected && delete_selected_diagram) {
        project.DeleteDiagram(index).Then(([]() { ne::NavigateToContent(); }));
      }

      ++index;
    }

    ImGui::EndTable();
  }
}
}  // namespace vh::ponc::draw