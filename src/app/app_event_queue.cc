#include "app_event_queue.h"

namespace esc {
void EventQueue::PostEvent(const Event &event) {
  events_.emplace_back(std::move(event));
}

void EventQueue::ExecuteEvents(StateNoQueue &state) {
  for (const auto &event : events_) {
    event(state);
  }

  events_.clear();
}
}  // namespace esc