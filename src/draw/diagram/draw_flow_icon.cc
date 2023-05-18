#include "draw_flow_icon.h"

#include "cpp_assert.h"
#include "imgui.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

namespace vh::ponc::draw {
namespace {
///
void DrawIconPath(ImDrawList& draw_list, ImVec2 pos, ImVec2 size) {
  const auto frame_padding = ImGui::GetStyle().FramePadding;
  const auto padding = ImVec2{frame_padding.x, frame_padding.x};

  pos += padding;
  size -= padding * 2;

  const auto top_left = pos;
  draw_list.PathLineTo(top_left);

  const auto top_middle = ImVec2{top_left.x + size.x / 2, top_left.y};
  draw_list.PathLineTo(top_middle);

  const auto arrow_tip = ImVec2{top_left.x + size.x, top_left.y + size.y / 2};
  draw_list.PathLineTo(arrow_tip);

  const auto bot_middle = ImVec2{top_middle.x, top_middle.y + size.y};
  draw_list.PathLineTo(bot_middle);

  const auto bot_left = ImVec2{pos.x, pos.y + size.y};
  draw_list.PathLineTo(bot_left);
}

///
void ExpandToMatchStrokedPath(ImVec2& pos, ImVec2& size, float thickness) {
  const auto half_thickness = thickness / 2;

  pos -= ImVec2{half_thickness, half_thickness};
  size += ImVec2{thickness, thickness};
}
}  // namespace

///
void DrawFlowIcon(ImVec2 size, const ImColor& color, bool filled) {
  auto* draw_list = ImGui::GetWindowDrawList();
  Expects(draw_list != nullptr);

  const auto thickness = 2;
  auto pos = ImGui::GetCursorScreenPos();

  if (filled) {
    ExpandToMatchStrokedPath(pos, size, thickness);
  }

  DrawIconPath(*draw_list, pos, size);

  if (filled) {
    draw_list->PathFillConvex(color);
    return;
  }

  draw_list->PathStroke(color, ImDrawFlags_Closed, thickness);
}
}  // namespace vh::ponc::draw