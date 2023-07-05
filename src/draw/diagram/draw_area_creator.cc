/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_area_creator.h"

#include <imgui.h>
#include <imgui_node_editor.h>

namespace ne = ax::NodeEditor;

namespace vh::ponc::draw {
void DrawAreaCreator(coreui::AreaCreator &area_creator) {
  const auto &start_pos = area_creator.GetStartPos();

  if (!start_pos.has_value()) {
    return;
  }

  const auto mouse_pos = ImGui::GetMousePos();

  if (ne::IsBackgroundClicked()) {
    if (ne::GetBackgroundClickButtonIndex() == ImGuiMouseButton_Left) {
      area_creator.AcceptCreateAreaAt(ne::ScreenToCanvas(mouse_pos));
    }
  }

  auto *draw_list = ImGui::GetWindowDrawList();
  Expects(draw_list != nullptr);

  draw_list->AddRect(ne::CanvasToScreen(*start_pos), mouse_pos,
                     ImColor{ne::GetStyle().Colors[ne::StyleColor_GroupBorder]},
                     ne::GetStyle().NodeRounding, 0,
                     ne::GetStyle().NodeBorderWidth);
}
}  // namespace vh::ponc::draw