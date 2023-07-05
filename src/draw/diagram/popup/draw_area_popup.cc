/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_area_popup.h"

#include <imgui.h>

#include "core_diagram.h"
#include "draw_rename_widget.h"

namespace vh::ponc::draw {
///
void AreaPopup::Draw(const std::vector<ne::NodeId> &selected_areas,
                     coreui::Diagram &diagram, const Callbacks &callbacks) {
  if (ImGui::MenuItem("Delete")) {
    for (const auto area_id : selected_areas) {
      diagram.DeleteArea(area_id);
    }
  }

  if (selected_areas.size() > 1) {
    return;
  }

  ImGui::Separator();

  auto &area =
      core::Diagram::FindArea(diagram.GetDiagram(), selected_areas.front());

  if (callbacks.was_just_opened()) {
    rename_buffer_.Set(area.name);
  }

  if (ImGui::BeginMenu("Rename")) {
    if (const auto rename_confirmed =
            DrawRenameWidget("##Area Name", rename_buffer_)) {
      area.name = rename_buffer_.AsTrimmed();
    }

    ImGui::EndMenu();
  }
}
}  // namespace vh::ponc::draw