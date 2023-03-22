#ifndef VH_PONC_CORE_GRADIENT_H_
#define VH_PONC_CORE_GRADIENT_H_

#include "imgui.h"

namespace vh::ponc::core {
///
auto GetGradient(const ImColor& start_color, const ImColor& end_color,
                 float percentage) -> ImColor;
}  // namespace vh::ponc::core

#endif  // VH_PONC_CORE_GRADIENT_H_
