#include "app_impl.h"

#include "coreui_project.h"

namespace esc {
///
AppImpl::AppImpl(coreui::TexturesHandle textures_handle)
    : project_{{}, std::move(textures_handle)} {}

///
void AppImpl::OnFrame() {
  project_.OnFrame();
  main_window_.Draw(project_);
}
}  // namespace esc