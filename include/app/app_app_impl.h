#ifndef VH_APP_APP_IMPL_H_
#define VH_APP_APP_IMPL_H_

#include <application.h>

#include "app_state.h"
#include "app_textures.h"
#include "imgui.h"

namespace esc {
class AppImpl {
 public:
  explicit AppImpl(const Textures &textures);

  // ---
  AppImpl(const AppImpl &) = delete;
  // ---
  AppImpl(AppImpl &&) noexcept = delete;

  // ---
  auto operator=(const AppImpl &) noexcept -> AppImpl & = delete;
  // ---
  auto operator=(AppImpl &&) noexcept -> AppImpl & = delete;

  // ---
  ~AppImpl();

  void OnFrame();

 private:
  State state_{};
  ne::EditorContext *editor_context_{};
};
}  // namespace esc

#endif  // VH_APP_APP_IMPL_H_
