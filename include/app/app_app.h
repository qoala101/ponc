#ifndef VH_APP_APP_H_
#define VH_APP_APP_H_

#include <application.h>

#include <optional>

#include "app_impl.h"
#include "cpp_safe_pointer.h"

namespace esc {
///
// NOLINTNEXTLINE(*-virtual-class-destructor, *-multiple-inheritance)
class App : public Application {
 public:
  ///
  App(const char* name, int argc, char** argv);

  ///
  auto GetWindowFlags() const -> ImGuiWindowFlags override;
  ///
  void OnStart() override;
  ///
  void OnStop() override;
  ///
  void OnFrame(float delta_time) override;

 private:
  ///
  auto LoadTexture(std::string_view file_path);

  ///
  cpp::SafePointerFactory safe_pointer_factory_{};
  ///
  std::optional<AppImpl> app_{};
};
}  // namespace esc

#endif  // VH_APP_APP_H_
