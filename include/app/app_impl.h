#ifndef VH_APP_IMPL_H_
#define VH_APP_IMPL_H_

#include <application.h>

#include "core_project.h"
#include "coreui_event_loop.h"
#include "coreui_textures_handle.h"
#include "draw_main_window.h"

namespace esc {
///
class AppImpl {
 public:
  ///
  explicit AppImpl(coreui::TexturesHandle textures_handle);

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
