#include "draw_open_file_dialog.h"

#include "draw_i_file_dialog.h"

namespace esc::draw {
OpenFileDialog::OpenFileDialog()
    : IFileDialog{[]() {
        auto dialog = ImGui::FileBrowser{ImGuiFileBrowserFlags_CloseOnEsc};
        dialog.SetTitle("Open Diagram JSON");
        dialog.SetTypeFilters({".json"});
        return dialog;
      }()} {}

void OpenFileDialog::OnFileSelected(std::string file_path) const {
  // app_->GetNodesAndLinks().LoadFromFile(selected_file_path);
}
}  // namespace esc::draw