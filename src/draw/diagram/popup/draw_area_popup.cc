/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_area_popup.h"

namespace vh::ponc::draw {
///
void AreaPopup::Draw(const std::vector<ne::NodeId> &selected_areas,
                     coreui::Diagram &diagram) {
  if (ImGui::MenuItem("Delete")) {
    for (const auto area_id : selected_areas) {
      diagram.DeleteArea(area_id);
    }
  }

  if (selected_areas.size() > 1) {
    return;
  }

  ImGui::Separator();

  if (ImGui::MenuItem("Rename")) {
    // TODO(vh)
  }

  if (ImGui::MenuItem("Unlock")) {
    // TODO(vh)
  }
}
}  // namespace vh::ponc::draw