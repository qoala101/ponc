/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_APP_EVENT_QUEUE_H_
#define VH_APP_EVENT_QUEUE_H_

#include <functional>

#include "app_state.h"

namespace esc {
using Event = std::function<void(const AppState &)>;

class EventQueue {
 public:
  void PostEvent(Event event);
  void ExecuteEvents(const AppState &app_state);

 private:
  std::vector<Event> events_{};
};
}  // namespace esc

#endif  // VH_APP_EVENT_QUEUE_H_
