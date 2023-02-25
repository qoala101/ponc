/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_DRAW_I_FILE_DIALOG_H_
#define VH_DRAW_I_FILE_DIALOG_H_

// clang-format off
#include <imgui.h>
#include <imfilebrowser.h>
// clang-format on

#include "cpp_interface.h"
#include "frame_node.h"

namespace esc::draw {
class IFileDialog : public cpp::Interface {
 public:
  void Show();
  void Draw(frame::Frame &frame);

 protected:
  explicit IFileDialog(const ImGui::FileBrowser &dialog);

 private:
  virtual void OnFileSelected(frame::Frame &frame,
                              std::string file_path) const = 0;

  ImGui::FileBrowser dialog_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_I_FILE_DIALOG_H_
