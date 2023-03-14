#ifndef VH_APP_IMPL_H_
#define VH_APP_IMPL_H_

#include <application.h>

#include "coreui_project.h"
#include "coreui_textures_handle.h"
#include "draw_main_window.h"

namespace esc {
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
}  // namespace esc

#endif  // VH_APP_IMPL_H_
