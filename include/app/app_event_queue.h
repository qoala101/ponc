/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_APP_EVENT_QUEUE_H_
#define VH_APP_EVENT_QUEUE_H_

#include <functional>

#include "core_project.h"

namespace esc {
// ---
using Event = std::function<void(core::Project &)>;

// ---
class EventQueue {
 public:
  // ---
  void PostEvent(Event event);
  // ---
  void ExecuteEvents(core::Project &project);

 private:
  // ---
  std::vector<Event> events_{};
};
}  // namespace esc

#endif  // VH_APP_EVENT_QUEUE_H_
