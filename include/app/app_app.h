/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_ESC_APP_H_
#define VH_ESC_APP_H_

#include <application.h>

#include <optional>

#include "app_impl.h"
#include "app_textures.h"

namespace esc {
// ---
// NOLINTNEXTLINE(*-virtual-class-destructor)
class App : public Application {
 public:
  // ---
  App(const char* name, int argc, char** argv);

  // ---
  auto GetWindowFlags [[nodiscard]] () const -> ImGuiWindowFlags override;
  // ---
  void OnStart() override;
  // ---
  void OnStop() override;
  // ---
  void OnFrame(float delta_time) override;

 private:
  // ---
  auto LoadTexture [[nodiscard]] (std::string_view file_path);

  // ---
  std::optional<Textures> textures_{};
  // ---
  std::optional<AppImpl> impl_{};
};
}  // namespace esc

#endif  // VH_ESC_APP_H_
