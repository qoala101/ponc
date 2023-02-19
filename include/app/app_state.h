/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_APP_STATE_H_
#define VH_APP_STATE_H_

namespace esc {
class EventQueue;

namespace core {
class Project;
class IdGenerator;
}  // namespace core

namespace flow {
class FlowCalculator;
}  // namespace flow

namespace draw {
struct Widgets;
}  // namespace draw

// ---
struct AppState {
  // ---
  core::Project *project{};
  // ---
  core::IdGenerator *id_generator{};
  // ---
  flow::FlowCalculator *flow_calculator{};
  // ---
  draw::Widgets *widgets{};
  // ---
  EventQueue *event_queue{};
};
}  // namespace esc

#endif  // VH_APP_STATE_H_
