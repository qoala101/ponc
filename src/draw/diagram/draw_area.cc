/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#define IMGUI_DEFINE_MATH_OPERATORS
#include "draw_area.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_node_editor.h>

#include "core_area.h"
#include "coreui_native_facade.h"
#include "coreui_node.h"
#include "draw_node.h"
#include "style_default_colors.h"

namespace vh::ponc::draw {
namespace {
///
auto DrawAreaHint(const core::Area &area) {
  auto opaque = false;

  if (ne::BeginGroupHint(area.id)) {
    const auto text_height =
        ImVec2{0, ImGui::CalcTextSize(area.name.c_str()).y};

    ImGui::SetCursorPos(ne::GetGroupMin() - text_height);
    ImGui::TextUnformatted(area.name.c_str());

    if (ImGui::GetStyle().Alpha >= 1) {
      opaque = true;
    }
  }

  ne::EndGroupHint();
  return opaque;
}
}  // namespace

///
void DrawArea(core::Area &area, coreui::NodeMover &node_mover) {
  const auto hint_opaque = DrawAreaHint(area);

  ImGui::PushID(area.id.AsPointer());
  ne::PushStyleColor(ne::StyleColor_NodeBg,
                     ImColor{style::DefaultColors::kTransparent});
  ne::PushStyleColor(ne::StyleColor_NodeBorder,
                     ne::GetStyle().Colors[ne::StyleColor_GroupBorder]);

  ne::BeginNode(area.id);
  DrawNodeHeader(
      coreui::Header{.label = hint_opaque ? std::string{} : area.name});
  ne::Group(area.size);
  ne::EndNode();

  ne::PopStyleColor();
  ne::PopStyleColor();
  ImGui::PopID();

  area.pos = ne::GetNodePosition(area.id);
  area.size = coreui::NativeFacade::GetAreaSize(area.id);
  node_mover.SetItemSize(area.id, area.size);
}
}  // namespace vh::ponc::draw