/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_APP_IMPL_H_
#define VH_APP_IMPL_H_

#include <application.h>

#include "app_event_queue.h"
#include "app_textures.h"
#include "core_project.h"
#include "draw_widgets.h"
#include "flow_calculator.h"

namespace esc {
// ---
class AppImpl {
 public:
  // ---
  explicit AppImpl(const Textures &textures);

  // ---
  void OnFrame();

 private:
  // ---
  core::Project project_{};
  // ---
  core::IdGenerator id_generator_{};
  // ---
  flow::FlowCalculator flow_calculator_{};
  // ---
  draw::Widgets widgets_;
  // ---
  EventQueue event_queue_{};
};
}  // namespace esc

#endif  // VH_APP_IMPL_H_
