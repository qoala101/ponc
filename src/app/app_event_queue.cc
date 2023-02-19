/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#include "app_event_queue.h"

namespace esc {
// ---
void EventQueue::PostEvent(Event event) {
  events_.emplace_back(std::move(event));
}

// ---
void EventQueue::ExecuteEvents(const AppState &app_state) {
  for (const auto &event : events_) {
    event(app_state);
  }

  events_.clear();
}
}  // namespace esc