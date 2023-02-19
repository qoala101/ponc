/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#include "app_event_queue.h"

#include "core_project.h"

namespace esc {
// ---
void EventQueue::PostEvent(Event event) {
  events_.emplace_back(std::move(event));
}

// ---
void EventQueue::ExecuteEvents(core::Project &project) {
  for (const auto &event : events_) {
    event(project);
  }

  events_.clear();
}
}  // namespace esc