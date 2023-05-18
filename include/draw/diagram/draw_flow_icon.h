/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_FLOW_ICON_H_
#define VH_PONC_DRAW_FLOW_ICON_H_

#include <imgui.h>

namespace vh::ponc::draw {
///
void DrawFlowIcon(ImVec2 size, const ImColor& color, bool filled);
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_FLOW_ICON_H_
