/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_area_creator.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_node_editor.h>

namespace ne = ax::NodeEditor;

namespace vh::ponc::draw {
namespace {
///
void ResizeArea(coreui::AreaCreator &area_creator) {
  const auto mouse_pos = ne::ScreenToCanvas(ImGui::GetMousePos());
  const auto padding = ne::GetStyle().NodePadding;

  area_creator.ResizeAreaTo(
      mouse_pos -
      ImVec2{2 * padding.x,
             2 * padding.y + ImGui::GetTextLineHeightWithSpacing()});
}
}  // namespace

///
void AreaCreator::Draw(coreui::AreaCreator &area_creator) {
  if (!area_creator.IsCreating()) {
    Reset();
    return;
  }

  if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) ||
      ImGui::IsMouseClicked(ImGuiMouseButton_Middle)) {
    area_creator.AcceptCreateArea();
    return;
  }

  if (ImGui::IsKeyPressedMap(ImGuiKey_Escape)) {
    area_creator.DiscardCreateArea();
    return;
  }

  if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
    right_mouse_clicked_ = true;
  }

  if (right_mouse_clicked_ && ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
    right_mouse_dragged_ = true;
  }

  if (!right_mouse_dragged_ && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
    area_creator.AcceptCreateArea();
    return;
  }

  ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
  ResizeArea(area_creator);
}

///
void AreaCreator::Reset() {
  right_mouse_clicked_ = false;
  right_mouse_dragged_ = false;
}
}  // namespace vh::ponc::draw