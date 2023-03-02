#include "app_impl.h"

#include "coreui_project.h"

namespace esc {
///
AppImpl::AppImpl(coreui::TexturesHandle textures_handle)
    : project_ui_{safe_pointer_factory_.CreateSafePointer(&project_),
                  {},
                  std::move(textures_handle)} {}

///
void AppImpl::OnFrame() {
  project_ui_.OnFrame();
  // main_window_.Draw(controller_);
}
}  // namespace esc