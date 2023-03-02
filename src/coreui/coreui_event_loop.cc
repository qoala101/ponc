#include "coreui_event_loop.h"

namespace esc::coreui {
///
EventLoop::EventLoop() : EnableSafePointer{this} {}

///
void EventLoop::PostEvent(Event event) {
  events_.emplace_back(std::move(event));
}

///
void EventLoop::ExecuteEvents() {
  for (const auto& event : events_) {
    event();
  }

  events_.clear();
}
}  // namespace esc::coreui