#ifndef VH_ESC_LEFT_PANE_H_
#define VH_ESC_LEFT_PANE_H_

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
  void DrawMenu(float pane_width);
  void DrawDialog();

  std::shared_ptr<App> app_;
  ImGui::FileBrowser open_file_dialog_{};
  ImGui::FileBrowser save_as_file_dialog_{};
};
}  // namespace esc

#endif  // VH_ESC_LEFT_PANE_H_
