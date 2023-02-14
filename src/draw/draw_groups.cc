#include "draw_groups.h"

#include <iostream>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include "imgui.h"
#include "imgui_node_editor.h"

namespace esc::draw {
void Groups::Draw(State& state) {
  const auto& groups = state.app_.GetDiagram().GetGroups();

  for (const auto& group : groups) {
    const auto color =
        ImColor{group.color_[0], group.color_[1], group.color_[2]};
    const auto nodes = group.GetNodes();

    auto rect = std::optional<ImRect>{};

    for (const auto& node : nodes) {
      const auto pos = ne::GetNodePosition(node->GetId());
      const auto size = ne::GetNodeSize(node->GetId());
      const auto rect_min = pos - ImVec2{10, 10};
      const auto rect_max = pos + size + ImVec2{10, 10};

      if (rect.has_value()) {
        rect->Min.x = std::min(rect_min.x, rect->Min.x);
        rect->Min.y = std::min(rect_min.y, rect->Min.y);
        rect->Max.x = std::max(rect_max.x, rect->Max.x);
        rect->Max.y = std::max(rect_max.y, rect->Max.y);
      } else {
        rect.emplace(rect_min, rect_max);
      }

      if (group.unite_) {
        continue;
      }

      auto* draw_list = ImGui::GetWindowDrawList();
      draw_list->AddRectFilled(rect_min, rect_max, color);
    }

    if (group.unite_ && rect.has_value()) {
      auto* draw_list = ImGui::GetWindowDrawList();
      draw_list->AddRectFilled(rect->Min, rect->Max, color);
    }
  }
}
}  // namespace esc::draw