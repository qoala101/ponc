#include "draw_flow_icon.h"

#include "cpp_assert.h"
#include "imgui.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

namespace esc::draw {
namespace {
void DrawIconPath(ImDrawList& draw_list, ImVec2 position, ImVec2 size) {
  const auto padding = ImVec2{4, 4};
  position += padding;
  size -= padding * 2;

  const auto top_left = position;
  draw_list.PathLineTo(top_left);

  const auto top_middle = ImVec2{top_left.x + size.x / 2, top_left.y};
  draw_list.PathLineTo(top_middle);

  const auto arrow_tip = ImVec2{top_left.x + size.x, top_left.y + size.y / 2};
  draw_list.PathLineTo(arrow_tip);

  const auto bottom_middle = ImVec2{top_middle.x, top_middle.y + size.y};
  draw_list.PathLineTo(bottom_middle);

  const auto bottom_left = ImVec2{position.x, position.y + size.y};
  draw_list.PathLineTo(bottom_left);
}
}  // namespace

void DrawFlowIcon(const ImVec2& size, const ImColor& color, bool filled) {
  auto* draw_list = ImGui::GetWindowDrawList();
  Expects(draw_list != nullptr);

  const auto position = ImGui::GetCursorScreenPos();

  if (filled) {
    DrawIconPath(*draw_list, position, size);
    draw_list->PathFillConvex(color);
  }

  DrawIconPath(*draw_list, position, size);
  draw_list->PathStroke(color, ImDrawFlags_Closed, 2.F);
}
}  // namespace esc::draw