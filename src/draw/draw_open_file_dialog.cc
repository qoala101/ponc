#include "draw_open_file_dialog.h"

#include "draw_i_file_dialog.h"
#include "esc_app_state.h"
#include "json_diagram_serializer.h"

namespace esc::draw {
OpenFileDialog::OpenFileDialog()
    : IFileDialog{[]() {
        auto dialog = ImGui::FileBrowser{ImGuiFileBrowserFlags_CloseOnEsc};
        dialog.SetTitle("Open Diagram JSON");
        dialog.SetTypeFilters({".json"});
        return dialog;
      }()} {}

void OpenFileDialog::OnFileSelected(AppState &app_state,
                                    std::string file_path) const {
  app_state.OpenDiagramFromFile(file_path);
}
}  // namespace esc::draw