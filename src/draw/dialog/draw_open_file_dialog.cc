#include "draw_open_file_dialog.h"

#include "app_events.h"
#include "app_state.h"
#include "draw_i_file_dialog.h"

namespace esc::draw {
OpenFileDialog::OpenFileDialog()
    : IFileDialog{[]() {
        auto dialog = ImGui::FileBrowser{ImGuiFileBrowserFlags_CloseOnEsc};
        dialog.SetTitle("Open Diagram JSON");
        dialog.SetTypeFilters({".json"});
        return dialog;
      }()} {}

void OpenFileDialog::OnFileSelected(const AppState &app_state,
                                    std::string file_path) const {
  state.event_queue->PostEvent(
      event::OpenDiagramFromFile{.file_path = std::move(file_path)});
}
}  // namespace esc::draw