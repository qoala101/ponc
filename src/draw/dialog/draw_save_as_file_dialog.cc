/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_save_as_file_dialog.h"

namespace vh::ponc::draw {
///
SaveAsFileDialog::SaveAsFileDialog()
    : IFileDialog{[]() {
        // NOLINTBEGIN(*-signed-bitwise)
        return ImGui::FileBrowser{ImGuiFileBrowserFlags_EnterNewFilename |
                                  ImGuiFileBrowserFlags_CreateNewDir |
                                  ImGuiFileBrowserFlags_CloseOnEsc};
        // NOLINTEND(*-signed-bitwise)
      }()} {}

///
auto SaveAsFileDialog::GetTitle() const -> std::string {
  return "Save Project As JSON";
}
}  // namespace vh::ponc::draw