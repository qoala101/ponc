/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_HELP_MARKER_H_
#define VH_PONC_DRAW_HELP_MARKER_H_

#include <imgui.h>

#include <string_view>

namespace vh::ponc::draw {
///
void DrawHelpMarker(std::string_view text);
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_HELP_MARKER_H_
