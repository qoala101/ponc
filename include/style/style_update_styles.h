/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_STYLE_UPDATE_STYLES_H_
#define VH_PONC_STYLE_UPDATE_STYLES_H_

#include <imgui.h>
#include <imgui_node_editor.h>

namespace ne = ax::NodeEditor;

namespace vh::ponc::style {
///
void UpdateStyle(ImGuiStyle &style);
///
void UpdateStyle(ne::Style &style);
}  // namespace vh::ponc::style

#endif  // VH_PONC_STYLE_UPDATE_STYLES_H_
