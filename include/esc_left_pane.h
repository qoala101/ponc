#ifndef VH_ESC_LEFT_PANEL_H_
#define VH_ESC_LEFT_PANEL_H_

// clang-format off
#include <imgui.h>
// clang-format on

#include <imfilebrowser.h>

class App;

namespace esc {
class LeftPane {
 public:
  explicit LeftPane(std::shared_ptr<App> app);

  void Draw(float pane_width);

 private:
  void DrawMenu();
  void DrawDialog();

  std::shared_ptr<App> app_;
  ImGui::FileBrowser file_browser_{};
};
}  // namespace esc

#endif  // VH_ESC_LEFT_PANEL_H_
