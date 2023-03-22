#ifndef VH_COREUI_EVENT_H_
#define VH_COREUI_EVENT_H_

#include <functional>
#include <memory>
#include <optional>
#include <utility>

namespace esc::coreui {
///
class Event {
 public:
  ///
  explicit Event(std::function<void()> event);

  ///
  void operator()() const;

  ///
  auto Then(std::function<void()> continuation) -> Event&;
  ///
  auto GetContinuation() -> std::optional<std::unique_ptr<Event>>&;

 private:
  ///
  std::function<void()> event_{};
  ///
  std::optional<std::unique_ptr<Event>> continuation_;
};
}  // namespace esc::coreui

#endif  // VH_COREUI_EVENT_H_
