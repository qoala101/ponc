#ifndef VH_COREUI_EVENT_LOOP_H_
#define VH_COREUI_EVENT_LOOP_H_

#include <functional>

#include "cpp_safe_pointer.h"

namespace esc::coreui {
class EventLoop : public cpp::EnableSafePointer<EventLoop> {
 public:
  EventLoop() : EnableSafePointer{this} {}

  std::vector<std::function<void()>> events_{};

  void OnFrame() {
    for (const auto& event : events_) {
      event();
    }

    events_.clear();
  }
};
}  // namespace esc::coreui

#endif  // VH_COREUI_EVENT_LOOP_H_
