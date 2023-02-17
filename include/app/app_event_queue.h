#ifndef VH_APP_EVENT_QUEUE_H_
#define VH_APP_EVENT_QUEUE_H_

#include <functional>

#include "core_state.h"
#include "draw_state.h"

namespace esc {
class StateNoQueue;

using Event = std::function<void(StateNoQueue &)>;

class EventQueue {
 public:
  void PostEvent(const Event &event);
  void ExecuteEvents(StateNoQueue &state);

 private:
  std::vector<Event> events_{};
};
}  // namespace esc

#endif  // VH_APP_EVENT_QUEUE_H_
