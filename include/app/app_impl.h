#ifndef VH_APP_IMPL_H_
#define VH_APP_IMPL_H_

#include <application.h>

#include "app_event_queue.h"
#include "app_textures.h"

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
  ne::EditorContext *editor_context_{};
  core::CoreState core_state_{};
  draw::DrawState draw_state{};
  EventQueue event_queue_{};
};
}  // namespace esc

#endif  // VH_APP_IMPL_H_
