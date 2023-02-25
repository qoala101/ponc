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

  //   // auto GetFreePinFamily  () const -> FreePinFamily &;
  //   // auto GetPlaceholderFamily  () const -> PlaceholderFamily
  //   &;

 private:
  core::IdGenerator id_generator_{};
  core::Project project_{};
  draw::MainWindow widgets_;
  //   // std::weak_ptr<FreePinFamily> free_pin_family_{};
  //   // std::weak_ptr<PlaceholderFamily> placeholder_family_{};
};
}  // namespace esc

#endif  // VH_APP_IMPL_H_
