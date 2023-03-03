#ifndef VH_DRAW_I_FILE_DIALOG_H_
#define VH_DRAW_I_FILE_DIALOG_H_

// clang-format off
#include <imgui.h>
#include <imfilebrowser.h>
#include <functional>
// clang-format on

#include "cpp_hook.h"
#include "cpp_interface.h"

namespace esc::draw {
///
class IFileDialog : public cpp::Interface {
 public:
  ///
  struct Hooks {
    ///
    cpp::Hook<void(std::string file_path)> file_selected{};
  };

  ///
  void Draw(const Hooks &hooks);
  ///
  void Show();

 protected:
  ///
  explicit IFileDialog(const ImGui::FileBrowser &dialog);

 private:
  ///
  ImGui::FileBrowser dialog_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_I_FILE_DIALOG_H_
