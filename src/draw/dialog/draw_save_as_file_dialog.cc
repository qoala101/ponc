#include "draw_save_as_file_dialog.h"

namespace vh::ponc::draw {
///
SaveAsFileDialog::SaveAsFileDialog()
    : IFileDialog{[]() {
        auto dialog =
            ImGui::FileBrowser{ImGuiFileBrowserFlags_EnterNewFilename |
                               ImGuiFileBrowserFlags_CreateNewDir |
                               ImGuiFileBrowserFlags_CloseOnEsc};
        dialog.SetTitle("Save Project As JSON");
        return dialog;
      }()} {}
}  // namespace vh::ponc::draw