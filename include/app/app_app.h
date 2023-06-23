/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_APP_APP_H_
#define VH_PONC_APP_APP_H_

#include <application.h>
#include <imgui.h>

#include <optional>

#include "app_impl.h"
#include "cpp_safe_ptr.h"

namespace vh::ponc {
///
// NOLINTNEXTLINE(*-virtual-class-destructor)
class App : public Application {
 public:
  ///
  App(const char* name, int argc, char** argv);

  ///
  auto GetWindowFlags() const -> ImGuiWindowFlags override;
  ///
  auto CanClose() -> bool override;
  ///
  void OnStart() override;
  ///
  void OnStop() override;
  ///
  void OnFrame(float delta_time) override;

 private:
  ///
  cpp::SafeOwner safe_owner_{};
  ///
  std::optional<AppImpl> app_{};
};
}  // namespace vh::ponc

#endif  // VH_PONC_APP_APP_H_
