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