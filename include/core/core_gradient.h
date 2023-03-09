#ifndef VH_CORE_GRADIENT_H_
#define VH_CORE_GRADIENT_H_

#include "imgui.h"

namespace esc::core {
///
auto GetGradient(const ImColor& start_color, const ImColor& end_color,
                 float percentage) -> ImColor;
}  // namespace esc::core

#endif  // VH_CORE_GRADIENT_H_
