/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#include "draw_save_as_file_dialog.h"

#include "app_event_queue.h"
#include "app_events.h"

namespace esc::draw {
namespace {
// ---
auto AsLowerCase(std::string text) {
  for (auto& character : text) {
    character = static_cast<char>(std::tolower(character));
  }

  return text;
}
}  // namespace

// ---
SaveAsFileDialog::SaveAsFileDialog()
    : IFileDialog{[]() {
        auto dialog =
            ImGui::FileBrowser{ImGuiFileBrowserFlags_EnterNewFilename |
                               ImGuiFileBrowserFlags_CreateNewDir |
                               ImGuiFileBrowserFlags_CloseOnEsc};
        dialog.SetTitle("Save Project As JSON");
        return dialog;
      }()} {}

// ---
void SaveAsFileDialog::OnFileSelected(const AppState& app_state,
                                      std::string file_path) const {
  if (const auto not_json_extension =
          !AsLowerCase(file_path).ends_with(".json")) {
    file_path += ".json";
  }

  app_state.event_queue->PostEvent(
      event::SaveProjectToFile{.file_path = std::move(file_path)});
}
}  // namespace esc::draw