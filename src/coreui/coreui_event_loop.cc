#include "coreui_event_loop.h"

namespace esc::coreui {
///
void EventLoop::PostEvent(Event event) {
  events_.emplace_back(std::move(event));
}

///
void EventLoop::ExecuteEvents() {
  auto events = std::move(events_);

  for (const auto& event : events) {
    event();
  }
}
}  // namespace esc::coreui