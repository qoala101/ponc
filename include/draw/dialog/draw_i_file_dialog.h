#ifndef VH_DRAW_I_FILE_DIALOG_H_
#define VH_DRAW_I_FILE_DIALOG_H_

// clang-format off
#include <imgui.h>
// clang-format on

#include <imfilebrowser.h>

#include "cpp_interface.h"

namespace esc {
class State;
}  // namespace esc

namespace esc::draw {
// ---
class IFileDialog : public cpp::Interface {
 public:
  // ---
  void Show();
  // ---
  void Draw(const AppState &app_state);

 protected:
  // ---
  explicit IFileDialog(const ImGui::FileBrowser &dialog);

 private:
  // ---
  virtual void OnFileSelected(const AppState &app_state,
                              std::string file_path) const = 0;

  ImGui::FileBrowser dialog_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_I_FILE_DIALOG_H_
