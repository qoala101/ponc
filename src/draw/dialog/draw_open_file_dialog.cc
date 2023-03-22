#include "draw_open_file_dialog.h"

#include "draw_i_file_dialog.h"

namespace vh::ponc::draw {
///
OpenFileDialog::OpenFileDialog()
    : IFileDialog{[]() {
        auto dialog = ImGui::FileBrowser{ImGuiFileBrowserFlags_CloseOnEsc};
        dialog.SetTitle("Open Project JSON");
        dialog.SetTypeFilters({".json"});
        return dialog;
      }()},
      confirmation_dialog_{
          {.title = "Open Project",
           .question = "Current project would not be saved.\nProceed?",
           .ok_label = "Open Project"}} {}

///
void OpenFileDialog::Draw(const Callbacks &callbacks,
                          bool requires_confirmation) {
  IFileDialog::Draw(
      {.file_selected = [&callbacks, requires_confirmation,
                         &selected_file_path = selected_file_path_,
                         &confirmation_dialog =
                             confirmation_dialog_](auto file_path) {
        if (!requires_confirmation) {
          callbacks.file_selected(std::move(file_path));
          return;
        }

        selected_file_path = std::move(file_path);
        confirmation_dialog.Open();
      }});

  confirmation_dialog_.Draw(
      {.accepted = [&callbacks, &selected_file_path = selected_file_path_]() {
        callbacks.file_selected(std::move(selected_file_path));
      }});
}
}  // namespace vh::ponc::draw