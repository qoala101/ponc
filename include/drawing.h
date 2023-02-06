#ifndef VH_DRAWING_H_
#define VH_DRAWING_H_

#include <imgui.h>

namespace ax::Drawing {

enum class IconType : ImU32 {
  Flow,
  Circle,
  Square,
  Grid,
  RoundSquare,
  Diamond
};

void DrawIcon(ImDrawList* drawList, const ImVec2& a, const ImVec2& b,
              IconType type, bool filled, ImU32 color, ImU32 innerColor);

}  // namespace ax::Drawing

#endif  // VH_DRAWING_H_
