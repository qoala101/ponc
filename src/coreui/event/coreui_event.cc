#include "coreui_event.h"

namespace vh::ponc::coreui {
///
Event::Event(std::function<void()> event) : event_{std::move(event)} {}

///
void Event::operator()() const { event_(); }

///
auto Event::Then(std::function<void()> continuation) -> Event& {
  continuation_ = std::make_unique<Event>(std::move(continuation));
  return **continuation_;
}

///
auto Event::GetContinuation() -> std::optional<std::unique_ptr<Event>>& {
  return continuation_;
}
}  // namespace vh::ponc::coreui