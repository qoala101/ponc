#ifndef VH_ESC_LEFT_PANEL_H_
#define VH_ESC_LEFT_PANEL_H_

// clang-format off
#include <imgui.h>
// clang-format on

#include <imfilebrowser.h>

class App;

namespace esc {
class LeftPanel {
 public:
  explicit LeftPanel(std::shared_ptr<App> app);

  void Draw(float panel_width);

 private:
  void DrawMenu();
  void DrawDialog();

  std::shared_ptr<App> app_;
  ImGui::FileBrowser file_browser_{};
};
}  // namespace esc

#endif  // VH_ESC_LEFT_PANEL_H_
