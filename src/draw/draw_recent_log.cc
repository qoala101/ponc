/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_recent_log.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <vector>

#include "draw_log_view.h"

namespace vh::ponc::draw {
///
void DrawRecentLog(const coreui::Log& log, const LogView& log_view) {
  if (log_view.IsOpened()) {
    return;
  }

  const auto messages = log.GetRecentMessages();

  if (messages.empty()) {
    return;
  }

  const auto* viewport = ImGui::GetMainViewport();
  const auto padding = ImGui::GetStyle().WindowPadding * 2;
  const auto window_pos =
      viewport->WorkPos + ImVec2{padding.x, viewport->WorkSize.y - padding.y};
  const auto window_pivot = ImVec2{0, 1};

  ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pivot);
  ImGui::SetNextWindowSizeConstraints(ImVec2{0, 0},
                                      viewport->WorkSize - padding * 2);

  // NOLINTBEGIN(*-signed-bitwise)
  auto window_flags = ImGuiWindowFlags_NoMove |
                      ImGuiWindowFlags_AlwaysAutoResize |
                      ImGuiWindowFlags_NoSavedSettings |
                      ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration;
  window_flags &= ~ImGuiWindowFlags_NoScrollbar;
  // NOLINTEND(*-signed-bitwise)

  if (ImGui::Begin("Recent Log", nullptr, window_flags)) {
    LogView::DrawMessages(log.GetRecentMessages(), false);
  }

  ImGui::End();
}
}  // namespace vh::ponc::draw