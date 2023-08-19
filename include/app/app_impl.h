/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_APP_IMPL_H_
#define VH_PONC_APP_IMPL_H_

#include "coreui_project.h"
#include "cpp_safe_ptr.h"
#include "draw_main_window.h"

namespace vh::ponc {
///
class AppImpl {
 public:
  ///
  AppImpl(coreui::Project::Callbacks project_callbacks,
          draw::MainWindow::Callbacks main_window_callbacks);

  ///
  AppImpl(const AppImpl &) = delete;
  ///
  AppImpl(AppImpl &&) noexcept = delete;

  ///
  auto operator=(const AppImpl &) -> AppImpl & = delete;
  ///
  auto operator=(AppImpl &&) noexcept -> AppImpl & = delete;

  ///
  ~AppImpl();

  ///
  void OnFrame();
  ///
  auto CanClose() -> bool;

 private:
  ///
  void OnFirstFrame();

  ///
  coreui::Project project_;
  ///
  draw::MainWindow main_window_{};
  ///
  draw::MainWindow::Callbacks main_window_callbacks_{};
};
}  // namespace vh::ponc

#endif  // VH_PONC_APP_IMPL_H_
