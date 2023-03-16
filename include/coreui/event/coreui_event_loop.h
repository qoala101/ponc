#ifndef VH_COREUI_EVENT_LOOP_H_
#define VH_COREUI_EVENT_LOOP_H_

#include <functional>

#include "coreui_event.h"

namespace esc::coreui {
///
class EventLoop {
 public:
  ///
  auto PostEvent(std::function<void()> event) -> Event&;
  ///
  void ExecuteEvents();

 private:
  ///
  std::vector<Event> events_{};
};
}  // namespace esc::coreui

#endif  // VH_COREUI_EVENT_LOOP_H_
