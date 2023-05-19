/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "coreui_event.h"

#include <utility>

namespace vh::ponc::coreui {
///
Event::Event(std::function<void()> event) : event_{std::move(event)} {}

///
void Event::operator()() const { event_(); }

///
auto Event::Then(std::function<void()> continuation) -> Event& {
  continuation_ = std::make_unique<Event>(std::move(continuation));
  return **continuation_;
}

///
auto Event::GetContinuation() -> std::optional<std::unique_ptr<Event>>& {
  return continuation_;
}
}  // namespace vh::ponc::coreui