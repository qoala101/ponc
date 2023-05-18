/**
 * PON Calculator https://github.com/qoala101/ponc
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_I_FILE_DIALOG_H_
#define VH_PONC_DRAW_I_FILE_DIALOG_H_

// clang-format off
#include <imgui.h>
#include <imgui_internal.h>
#include <imfilebrowser.h>
// clang-format on

#include <functional>

#include "cpp_callbacks.h"
#include "cpp_non_copyable.h"

namespace vh::ponc::draw {
///
class IFileDialog : public cpp::NonCopyable {
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
  virtual auto GetTitle() const -> std::string = 0;

  ///
  void FixWindowFlags();

  ///
  ImGui::FileBrowser dialog_{};
  ///
  ImGuiWindow *dialog_window_{};
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_I_FILE_DIALOG_H_
