/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_settings_table_row.h"

#include <imgui.h>

#include <limits>

namespace vh::ponc::draw {
///
void DrawSettingsTableRow(std::string_view label) {
  ImGui::TableNextRow();

  ImGui::TableNextColumn();
  ImGui::AlignTextToFramePadding();
  ImGui::TextUnformatted(label.data());

  ImGui::TableNextColumn();
  ImGui::SetNextItemWidth(-std::numeric_limits<float>::min());
}
}  // namespace vh::ponc::draw