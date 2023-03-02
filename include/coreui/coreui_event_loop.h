#ifndef VH_COREUI_EVENT_LOOP_H_
#define VH_COREUI_EVENT_LOOP_H_

#include <functional>

#include "cpp_safe_pointer.h"

namespace esc::coreui {
///
using Event = std::function<void()>;

///
class EventLoop : public cpp::EnableSafePointer<EventLoop> {
 public:
  ///
  EventLoop();

  ///
  void PostEvent(Event event);
  ///
  void ExecuteEvents();

 private:
  ///
  std::vector<Event> events_{};
};
}  // namespace esc::coreui

#endif  // VH_COREUI_EVENT_LOOP_H_
