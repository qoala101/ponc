/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_colored_text.h"
#include "style_default_colors.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_node_editor.h>

#include "core_area.h"
#include "coreui_native_facade.h"
#include "draw_area.h"

namespace vh::ponc::draw {
namespace {
///
void DrawAreaName(const core::Area &area) {
  const auto color = ImColor{0, 0, 0, 255};
  auto pos = ImVec2{0, -ImGui::CalcTextSize(area.name.c_str()).y};

  if (ne::BeginGroupHint(area.id)) {
    DrawColoredText(area.name, color, pos + ne::GetGroupMin());
    ne::EndGroupHint();
  } else {
    DrawColoredText(area.name, color, pos + area.pos);
  }
}
}  // namespace

///
void DrawArea(core::Area &area, coreui::NodeMover &node_mover) {
  ImGui::PushID(area.id.AsPointer());

  ne::BeginNode(area.id);
  ne::Group(area.size);
  ne::EndNode();

  ImGui::PopID();

  DrawAreaName(area);

  area.pos = ne::GetNodePosition(area.id);
  area.size = coreui::NativeFacade::GetAreaSize(area.id);
  node_mover.SetItemSize(area.id, area.size);
}
}  // namespace vh::ponc::draw