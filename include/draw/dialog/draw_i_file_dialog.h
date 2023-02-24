/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_DRAW_I_FILE_DIALOG_H_
#define VH_DRAW_I_FILE_DIALOG_H_

// clang-format off
#include <imgui.h>
#include <imfilebrowser.h>
// clang-format on

#include "app_state.h"
#include "cpp_interface.h"

namespace esc::draw {
class IFileDialog : public cpp::Interface {
 public:
  void Show();
  void Draw(const AppState &app_state);

 protected:
  explicit IFileDialog(const ImGui::FileBrowser &dialog);

 private:
  virtual void OnFileSelected(const AppState &app_state,
                              std::string file_path) const = 0;

  ImGui::FileBrowser dialog_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_I_FILE_DIALOG_H_
