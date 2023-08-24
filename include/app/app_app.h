/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_APP_APP_H_
#define VH_PONC_APP_APP_H_

#include <imgui.h>

#include <optional>

#include "app_impl.h"
#include "cpp_safe_ptr.h"

namespace vh::ponc {
///
// NOLINTNEXTLINE(*-virtual-class-destructor)
class App {
 public:
  ///
  App(const char* name, int argc, char** argv);

  ///
  auto GetWindowFlags() const -> ImGuiWindowFlags;
  ///
  auto CanClose() -> bool;
  ///
  void OnStart();
  ///
  void OnStop();
  ///
  void OnFrame(float delta_time);

 private:
  ///
  cpp::SafeOwner safe_owner_{};
  ///
  std::optional<AppImpl> app_{};
};
}  // namespace vh::ponc

#endif  // VH_PONC_APP_APP_H_
