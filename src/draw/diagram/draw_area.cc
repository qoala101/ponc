/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_area.h"

#include <imgui.h>
#include <imgui_node_editor.h>

#include "core_area.h"
#include "coreui_native_facade.h"

namespace vh::ponc::draw {
///
void DrawArea(core::Area &area, coreui::NodeMover &node_mover) {
  ImGui::PushID(area.id.AsPointer());

  ne::BeginNode(area.id);
  ne::Group(area.size);
  ne::EndNode();

  ImGui::PopID();

  area.pos = ne::GetNodePosition(area.id);
  area.size = coreui::NativeFacade::GetAreaSize(area.id);
  node_mover.SetItemSize(area.id, area.size);
}
}  // namespace vh::ponc::draw