#include "draw_log_view.h"

#include <imgui.h>

#include <ctime>
#include <limits>

#include "coreui_log.h"

namespace vh::ponc::draw {
namespace {
///
auto ToString(std::chrono::time_point<std::chrono::system_clock> time) {
  const auto time_t = std::chrono::system_clock::to_time_t(time);
  const auto* local_time = std::localtime(&time_t);

  auto text = std::string{"HH:MM:SS"};
  std::ignore = std::strftime(text.data(), text.size() + 1, "%T", local_time);

  return text;
}

///
auto GetLogColor(coreui::LogLevel log_level) {
  switch (log_level) {
    case coreui::LogLevel::kInfo:
      return ImColor{0.5F, 0.5F, 1.F};
    case coreui::LogLevel::kWarning:
      return ImColor{1.F, 1.F, 0.5F};
    case coreui::LogLevel::kError:
      return ImColor{1.F, 0.5F, 0.5F};
  }
}

///
auto ToString(coreui::LogLevel log_level) {
  switch (log_level) {
    case coreui::LogLevel::kInfo:
      return "[info]";
    case coreui::LogLevel::kWarning:
      return "[warning]";
    case coreui::LogLevel::kError:
      return "[error]";
  }
}

///
void AutoScrollAtBottom() {
  if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
    ImGui::SetScrollHereY(1);
  }
}
}  // namespace

///
void LogView::DrawMessages(const std::vector<coreui::LogMessage>& messages,
                           bool wrap_text) {
  auto layout_id = 0;

  for (const auto& message : messages) {
    ImGui::BeginHorizontal(layout_id);
    ImGui::TextUnformatted(ToString(message.time).c_str());
    ImGui::TextColored(GetLogColor(message.level), ToString(message.level));

    if (wrap_text) {
      ImGui::TextWrapped(message.text.c_str());
    } else {
      ImGui::TextUnformatted(message.text.c_str());
    }

    ImGui::EndHorizontal();
  }
}

///
auto LogView::GetLabel() const -> std::string { return "Messages"; }

///
void LogView::Draw(const coreui::Log& log) {
  const auto content_scope = DrawContentScope();

  if (!IsOpened()) {
    return;
  }

  DrawMessages(log.GetMessages(), true);
  AutoScrollAtBottom();
}
}  // namespace vh::ponc::draw