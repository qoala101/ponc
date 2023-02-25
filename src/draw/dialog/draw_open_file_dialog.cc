/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#include "draw_open_file_dialog.h"

#include "draw_i_file_dialog.h"
#include "frame_node.h"

namespace esc::draw {
OpenFileDialog::OpenFileDialog()
    : IFileDialog{[]() {
        auto dialog = ImGui::FileBrowser{ImGuiFileBrowserFlags_CloseOnEsc};
        dialog.SetTitle("Open Project JSON");
        dialog.SetTypeFilters({".json"});
        return dialog;
      }()} {}

void OpenFileDialog::OnFileSelected(frame::Frame &frame,
                                    std::string file_path) const {
  frame.OpenProjectFromFile(std::move(file_path));
}
}  // namespace esc::draw