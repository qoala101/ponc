#include "coreui_log.h"

#include <algorithm>
#include <chrono>
#include <iterator>

namespace vh::ponc::coreui {
///
void Log::Write(LogLevel level, std::string text) {
  messages_.emplace_back(LogMessage{.time = std::chrono::system_clock::now(),
                                    .level = level,
                                    .text = std::move(text)});
}

///
auto Log::GetMessages() const -> const std::vector<LogMessage>& {
  return messages_;
}

///
auto Log::GetRecentMessages() const -> std::vector<LogMessage> {
  auto recent_messages = std::vector<LogMessage>{};

  std::copy_if(
      messages_.cbegin(), messages_.cend(), std::back_inserter(recent_messages),
      [now = std::chrono::system_clock::now()](const auto& message) {
        static constexpr auto kRecentDuration = std::chrono::seconds{5};

        const auto lifetime = now - message.time;
        return lifetime < kRecentDuration;
      });

  return recent_messages;
}
}  // namespace vh::ponc::coreui