/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_APP_IMPL_H_
#define VH_PONC_APP_IMPL_H_

#include "coreui_project.h"
#include "draw_main_window.h"

namespace vh::ponc {
///
class AppImpl {
 public:
  ///
  explicit AppImpl(coreui::Project::Callbacks project_callbacks);

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
