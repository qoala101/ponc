/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_help_marker.h"

#include <imgui.h>

namespace vh::ponc::draw {
///
void DrawHelpMarker(std::string_view text) {
  ImGui::TextDisabled("(?)");

  if (!ImGui::IsItemHovered()) {
    return;
  }

  ImGui::BeginTooltip();
  ImGui::TextUnformatted(text.data());
  ImGui::EndTooltip();
}
}  // namespace vh::ponc::draw