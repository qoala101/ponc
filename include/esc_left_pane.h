#ifndef VH_ESC_LEFT_PANE_H_
#define VH_ESC_LEFT_PANE_H_

// clang-format off
#include <imgui.h>
// clang-format on

#include <imfilebrowser.h>

namespace esc {
class App;

class LeftPane {
 public:
  explicit LeftPane(std::shared_ptr<App> app);

  void Draw(float pane_width);

 private:
  void DrawMenu(float pane_width);

  std::shared_ptr<App> app_;
};
}  // namespace esc

#endif  // VH_ESC_LEFT_PANE_H_
