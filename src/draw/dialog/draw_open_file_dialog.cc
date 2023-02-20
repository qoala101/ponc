/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#include "draw_open_file_dialog.h"

#include "app_event_queue.h"
#include "app_events.h"
#include "app_state.h"
#include "draw_i_file_dialog.h"

namespace esc::draw {
// ---
OpenFileDialog::OpenFileDialog()
    : IFileDialog{[]() {
        auto dialog = ImGui::FileBrowser{ImGuiFileBrowserFlags_CloseOnEsc};
        dialog.SetTitle("Open Project JSON");
        dialog.SetTypeFilters({".json"});
        return dialog;
      }()} {}

// ---
void OpenFileDialog::OnFileSelected(const AppState &app_state,
                                    std::string file_path) const {
  app_state.event_queue->PostEvent(
      event::OpenProjectFromFile{.file_path = std::move(file_path)});
}
}  // namespace esc::draw