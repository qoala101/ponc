/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_APP_STATE_H_
#define VH_APP_STATE_H_

namespace esc {
class EventQueue;

namespace core {
class IdGenerator;
class Project;
}  // namespace core

namespace draw {
struct Widgets;
}  // namespace draw

namespace frame {
struct Frame;
}  // namespace frame

// ---
struct AppState {
  // ---
  core::IdGenerator &id_generator;
  // ---
  core::Project &project;
  // ---
  draw::Widgets &widgets;
  // ---
  EventQueue &event_queue;

  frame::Frame &frame;
};
}  // namespace esc

#endif  // VH_APP_STATE_H_
