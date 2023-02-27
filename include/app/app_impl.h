#ifndef VH_APP_IMPL_H_
#define VH_APP_IMPL_H_

#include <application.h>

#include "app_textures.h"
#include "core_project.h"
#include "draw_main_window.h"

namespace esc {
class AppImpl {
 public:
  explicit AppImpl(const Textures &textures);

  void OnFrame();

 private:
  core::Project project_{};
  coreui::Frame controller_;
  draw::MainWindow main_window_{};
};
}  // namespace esc

#endif  // VH_APP_IMPL_H_
