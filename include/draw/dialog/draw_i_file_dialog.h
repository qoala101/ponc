#ifndef VH_PONC_DRAW_I_FILE_DIALOG_H_
#define VH_PONC_DRAW_I_FILE_DIALOG_H_

// clang-format off
#include <imgui.h>
#include <imfilebrowser.h>
// clang-format on

#include <functional>

#include "cpp_callbacks.h"
#include "cpp_interface.h"

namespace vh::ponc::draw {
///
class IFileDialog : public cpp::Interface {
 public:
  ///
  struct Callbacks {
    ///
    cpp::Signal<std::filesystem::path> file_selected{};
  };

  ///
  void Open();
  ///
  void Draw(const Callbacks &callbacks);

 protected:
  ///
  explicit IFileDialog(const ImGui::FileBrowser &dialog);

 private:
  ///
  ImGui::FileBrowser dialog_{};
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_I_FILE_DIALOG_H_
