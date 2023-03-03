#include "draw_open_file_dialog.h"

#include "draw_i_file_dialog.h"

namespace esc::draw {
///
OpenFileDialog::OpenFileDialog()
    : IFileDialog{[]() {
        auto dialog = ImGui::FileBrowser{ImGuiFileBrowserFlags_CloseOnEsc};
        dialog.SetTitle("Open Project JSON");
        dialog.SetTypeFilters({".json"});
        return dialog;
      }()} {}
}  // namespace esc::draw