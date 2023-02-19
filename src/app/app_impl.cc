/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#include "app_impl.h"

#include "draw_widgets.h"

namespace esc {
AppImpl::AppImpl(const Textures &textures) : widgets_{textures.node_header} {}

void AppImpl::OnFrame() {
  event_queue_.ExecuteEvents(project_);
  flow_calculator_.Recalculate(project_);
  widgets_.Draw(project_, id_generator_, flow_calculator_, event_queue_);
}
}  // namespace esc