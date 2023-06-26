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
#include "core_i_node.h"

namespace vh::ponc::draw {
// namespace {
// void DrawAreaZoomedOutLabel(core::INode &area_node) {
//   if (ne::BeginGroupHint(area_node.GetId())) {
//     auto bgAlpha = static_cast<int>(ImGui::GetStyle().Alpha * 255);

//     auto min = ne::GetGroupMin();
//     // auto max = ne::GetGroupMax();

//     ImGui::SetCursorScreenPos(
//         min - ImVec2(-8, ImGui::GetTextLineHeightWithSpacing() + 4));
//     ImGui::BeginGroup();
//     ImGui::TextUnformatted("Node name");
//     ImGui::EndGroup();

//     auto drawList = ne::GetHintBackgroundDrawList();

//     auto hintBounds = ImRect{ImGui::GetItemRectMin(), ImGui::GetItemRectMax()};
//     auto hintFrameBounds = hintBounds;
//     hintFrameBounds.Expand(ImVec2{8, 4});

//     drawList->AddRectFilled(hintFrameBounds.GetTL(), hintFrameBounds.GetBR(),
//                             IM_COL32(255, 255, 255, 64 * bgAlpha / 255), 4.0f);

//     drawList->AddRect(hintFrameBounds.GetTL(), hintFrameBounds.GetBR(),
//                       IM_COL32(255, 255, 255, 128 * bgAlpha / 255), 4.0f);
//   }

//   ne::EndGroupHint();
// }
// }  // namespace

void DrawArea(core::Area &area, coreui::NodeMover &node_mover) {
  const float commentAlpha = 0.75f;
  const auto node_id = area.id;
  const auto node_size = area.size;

  ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAlpha);
  ne::PushStyleColor(ne::StyleColor_NodeBg, ImColor(255, 255, 255, 64));
  ne::PushStyleColor(ne::StyleColor_NodeBorder, ImColor(255, 255, 255, 64));

  ne::BeginNode(node_id);
  ImGui::PushID(node_id.AsPointer());
  ImGui::BeginVertical("content");
  ImGui::BeginHorizontal("horizontal");
  ImGui::Spring(1);
  ImGui::TextUnformatted(area.name.c_str());
  ImGui::Spring(1);
  ImGui::EndHorizontal();
  ne::Group(node_size);
  ImGui::EndVertical();
  ImGui::PopID();
  ne::EndNode();
  ne::PopStyleColor(2);
  ImGui::PopStyleVar();

  area.pos = ne::GetNodePosition(node_id);
  node_mover.SetItemSize(node_id, ne::GetNodeSize(node_id));
}
}  // namespace vh::ponc::draw