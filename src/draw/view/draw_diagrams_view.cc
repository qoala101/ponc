#include "draw_diagrams_view.h"

#include <imgui.h>

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <set>
#include <string>
#include <unordered_set>

#include "core_diagram.h"
#include "cpp_scope.h"
#include "cpp_scope_function.h"
#include "draw_disable_if.h"
#include "draw_id_label.h"
#include "draw_table_flags.h"

namespace vh::ponc::draw {
///
auto DiagramsView::GetLabel() const -> std::string { return "Diagrams"; }

///
void DiagramsView::Draw(coreui::Project& project) {
  const auto content_scope = DrawContentScope();

  if (!IsOpened()) {
    return;
  }

  const auto& diagrams = project.GetProject().GetDiagrams();

  auto delete_diagram = false;
  auto clone_diagram = false;
  auto start_renaming_diagram = false;
  auto rename_diagram = false;

  ImGui::BeginHorizontal("Controls");

  {
    const auto disable_scope = DisableIf(diagrams.size() <= 1);

    if (ImGui::Button("Delete")) {
      delete_diagram = true;
    }
  }

  if (ImGui::Button("Create")) {
    project.AddDiagram(core::Diagram{"New Diagram"});
  }

  if (ImGui::Button("Clone")) {
    clone_diagram = true;
  }

  if (ImGui::Button("Rename")) {
    start_renaming_diagram = true;
    ImGui::OpenPopup("Rename Diagram");
  }

  // NOLINTNEXTLINE(*-redundant-string-cstr)
  auto trimmed_rename_buffer = std::string{rename_buffer_.c_str()};

  if (ImGui::BeginPopup("Rename Diagram")) {
    ImGui::InputText("##Diagram Name", rename_buffer_.data(),
                     rename_buffer_.size());
    ImGui::SameLine();

    {
      const auto disable_scope = DisableIf(trimmed_rename_buffer.empty());

      if (ImGui::Button("OK")) {
        rename_diagram = true;
        ImGui::CloseCurrentPopup();
      }
    }

    ImGui::EndPopup();
  }

  ImGui::EndHorizontal();

  // NOLINTBEGIN(*-signed-bitwise)
  const auto table_flags = ImGuiTableFlags_BordersH |
                           ImGuiTableFlags_BordersOuter |
                           ImGuiTableFlags_ScrollY;
  // NOLINTEND(*-signed-bitwise)

  if (ImGui::BeginTable("Diagrams", 1, table_flags)) {
    auto& diagrams = project.GetProject().GetDiagrams();

    for (auto i = 0; i < static_cast<int>(diagrams.size()); ++i) {
      ImGui::TableNextRow();
      ImGui::TableNextColumn();

      auto& diagram = diagrams[i];
      const auto item_is_selected =
          &diagram == &project.GetDiagram().GetDiagram();

      if (ImGui::Selectable(IdLabel(i, diagram.GetName()).c_str(),
                            item_is_selected) &&
          !item_is_selected) {
        project.SetDiagram(i);
      }

      if (!item_is_selected) {
        continue;
      }

      if (delete_diagram) {
        project.DeleteDiagram(i);
        continue;
      }

      if (clone_diagram) {
        project.CloneDiagram(diagram);
        continue;
      }

      if (start_renaming_diagram) {
        rename_buffer_ = diagram.GetName();

        static constexpr auto kMaxNameLength = 256;
        rename_buffer_.resize(kMaxNameLength);
        continue;
      }

      if (rename_diagram) {
        diagram.SetName(std::move(trimmed_rename_buffer));
        continue;
      }
    }

    ImGui::EndTable();
  }
}
}  // namespace vh::ponc::draw