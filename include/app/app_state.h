#ifndef VH_APP_STATE_H_
#define VH_APP_STATE_H_

#include "app_event_queue.h"
#include "core_state.h"
#include "draw_state.h"

namespace esc {
class StateNoQueue {
 public:
  core::CoreState *core_state{};
  // draw::DrawState *draw_state{};
};

class State {
 public:
  // const
  core::CoreState *core_state{};
  draw::DrawState *draw_state{};
  EventQueue *event_queue{};

  auto ToStateNoQueue() const {
    return StateNoQueue{
        core_state
        // , draw_state
    };
  }
};
}  // namespace esc

#endif  // VH_APP_STATE_H_
