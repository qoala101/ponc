#include "coreui_log.h"

#include <chrono>

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
}  // namespace vh::ponc::coreui