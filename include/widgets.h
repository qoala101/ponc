#ifndef VH_WIDGETS_H_
#define VH_WIDGETS_H_

#include <imgui.h>

#include "esc_types.h"

namespace ax::Widgets {

using Drawing::IconType;

void Icon(const ImVec2& size, IconType type, bool filled,
          const ImVec4& color = ImVec4(1, 1, 1, 1),
          const ImVec4& innerColor = ImVec4(0, 0, 0, 0));

}  // namespace ax::Widgets

#endif  // VH_WIDGETS_H_
