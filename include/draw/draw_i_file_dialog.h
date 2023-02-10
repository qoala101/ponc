#ifndef VH_DRAW_I_FILE_DIALOG_H_
#define VH_DRAW_I_FILE_DIALOG_H_

// clang-format off
#include <imgui.h>
// clang-format on

#include <imfilebrowser.h>

#include "draw_i_drawable.h"

namespace esc::draw {
class IFileDialog : public IDrawable {
 public:
  void Draw() override;

 protected:
  explicit IFileDialog(const ImGui::FileBrowser &dialog);

 private:
  void OnVisibleChanged() override;

  virtual void OnFileSelected(std::string file_path) const = 0;

  ImGui::FileBrowser dialog_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_I_FILE_DIALOG_H_
