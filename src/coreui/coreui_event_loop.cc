#include "coreui_event_loop.h"

namespace esc::coreui {
///
void EventLoop::PostEvent(Event event) {
  events_.emplace_back(std::move(event));
}

///
void EventLoop::PostLateEvent(Event event) {
  late_events_.emplace_back(std::move(event));
}

///
void EventLoop::ExecuteEvents() {
  for (const auto& event : events_) {
    event();
  }

  events_ = std::move(late_events_);
}
}  // namespace esc::coreui