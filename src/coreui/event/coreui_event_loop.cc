/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "coreui_event_loop.h"

#include <memory>
#include <optional>
#include <utility>

namespace vh::ponc::coreui {
///
auto EventLoop::PostEvent(std::function<void()> event) -> Event& {
  return events_.emplace_back(std::move(event));
}

///
void EventLoop::ExecuteEvents() {
  auto events = std::move(events_);

  for (auto& event : events) {
    event();

    if (auto& continuation = event.GetContinuation()) {
      events_.emplace_back(std::move(**continuation));
    }
  }
}
}  // namespace vh::ponc::coreui