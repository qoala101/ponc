#ifndef VH_PONC_APP_IMPL_H_
#define VH_PONC_APP_IMPL_H_

#include <application.h>

#include "coreui_project.h"
#include "coreui_textures_handle.h"
#include "draw_main_window.h"

namespace vh::ponc {
///
class AppImpl {
 public:
  ///
  AppImpl(coreui::TexturesHandle textures_handle,
          coreui::Project::Callbacks project_callbacks);

  ///
  void OnFrame();

 private:
  ///
  coreui::Project project_;
  ///
  draw::MainWindow main_window_{};
};
}  // namespace vh::ponc

#endif  // VH_PONC_APP_IMPL_H_
