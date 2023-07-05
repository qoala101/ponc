/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_area.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_node_editor.h>

#include "core_area.h"
#include "coreui_native_facade.h"
#include "cpp_assert.h"
#include "style_default_colors.h"

namespace vh::ponc::draw {
namespace {
///
auto DrawAreaHint(const core::Area &area) {
  auto alpha = 0.F;

  if (ne::BeginGroupHint(area.id)) {
    const auto text_height =
        ImVec2{0, ImGui::CalcTextSize(area.name.c_str()).y};

    ImGui::SetCursorPos(ne::GetGroupMin() - text_height);
    ImGui::TextUnformatted(area.name.c_str());

    alpha = ImGui::GetStyle().Alpha;
  }

  ne::EndGroupHint();
  return alpha;
}

///
void DrawAreaHeader(const core::Area &area, float alpha) {
  ImGui::BeginHorizontal("Header");
  ImGui::Spring(0);

  const auto padding = ImGui::GetItemRectSize().x;
  const auto start_pos = ImGui::GetCursorPos();

  ImGui::TextUnformatted("");
  ImGui::EndHorizontal();

  auto *draw_list = ImGui::GetWindowDrawList();
  Expects(draw_list != nullptr);

  const auto end_pos =
      start_pos + area.size + ImVec2{-2 * padding, ImGui::GetTextLineHeight()};

  ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
  ImGui::RenderTextEllipsis(draw_list, start_pos, end_pos, end_pos.x, end_pos.x,
                            area.name.c_str(), nullptr, nullptr);
  ImGui::PopStyleVar();
}

///
void UpdateArea(core::Area &area, coreui::NodeMover &node_mover) {
  area.pos = ne::GetNodePosition(area.id);
  area.size = coreui::NativeFacade::GetAreaSize(area.id);
  node_mover.SetItemSize(area.id, area.size);
}
}  // namespace

///
void DrawArea(core::Area &area, coreui::NodeMover &node_mover) {
  const auto hint_alpha = DrawAreaHint(area);

  ImGui::PushID(area.id.AsPointer());
  ne::PushStyleColor(ne::StyleColor_NodeBg,
                     ImColor{style::DefaultColors::kTransparent});
  ne::PushStyleColor(ne::StyleColor_NodeBorder,
                     ne::GetStyle().Colors[ne::StyleColor_GroupBorder]);

  ne::BeginNode(area.id);

  const auto header_alpha = 1.F - hint_alpha;
  DrawAreaHeader(area, header_alpha);

  ne::Group(area.size);
  ne::EndNode();

  ne::PopStyleColor();
  ne::PopStyleColor();
  ImGui::PopID();

  UpdateArea(area, node_mover);
}
}  // namespace vh::ponc::draw