/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_ESC_APP_H_
#define VH_ESC_APP_H_

#include <application.h>

#include <memory>
#include <optional>

#include "esc_app_impl.h"
#include "imgui.h"

namespace ax::NodeEditor {
using Application = Application;
}  // namespace ax::NodeEditor

namespace ne = ax::NodeEditor;

namespace esc {
// ---
// NOLINTNEXTLINE(*-virtual-class-destructor)
class App : public ne::Application {
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
  std::optional<AppImpl> impl_{};
};
}  // namespace esc

#endif  // VH_ESC_APP_H_
