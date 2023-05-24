/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_COREUI_EVENT_LOOP_H_
#define VH_PONC_COREUI_EVENT_LOOP_H_

#include <functional>
#include <vector>

#include "coreui_event.h"

namespace vh::ponc::coreui {
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
}  // namespace vh::ponc::coreui

#endif  // VH_PONC_COREUI_EVENT_LOOP_H_
