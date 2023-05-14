#ifndef VH_PONC_COREUI_LOG_H_
#define VH_PONC_COREUI_LOG_H_

#include <chrono>
#include <string>
#include <vector>

namespace vh::ponc::coreui {
///
enum class LogLevel { kInfo, kError, kDone };

///
struct LogMessage {
  ///
  std::chrono::time_point<std::chrono::system_clock> time{};
  ///
  LogLevel level{};
  ///
  std::string text{};
};

///
class Log {
 public:
  ///
  void Write(LogLevel level, std::string text);
  ///
  auto GetMessages() const -> const std::vector<LogMessage> &;
  ///
  auto GetRecentMessages() const -> std::vector<LogMessage>;

 private:
  ///
  std::vector<LogMessage> messages_{};
};
}  // namespace vh::ponc::coreui

#endif  // VH_PONC_COREUI_LOG_H_
