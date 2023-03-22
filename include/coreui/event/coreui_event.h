#ifndef VH_PONC_COREUI_EVENT_H_
#define VH_PONC_COREUI_EVENT_H_

#include <functional>
#include <memory>
#include <optional>
#include <utility>

namespace vh::ponc::coreui {
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
}  // namespace vh::ponc::coreui

#endif  // VH_PONC_COREUI_EVENT_H_
