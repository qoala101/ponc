/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_open_file_dialog.h"

#include "draw_i_file_dialog.h"

namespace vh::ponc::draw {
///
OpenFileDialog::OpenFileDialog()
    : IFileDialog{[]() {
        auto dialog = ImGui::FileBrowser{ImGuiFileBrowserFlags_CloseOnEsc};
        dialog.SetTypeFilters({".json"});
        return dialog;
      }()},
      confirmation_dialog_{
          {.title = "Open Project",
           .question = "Current project would not be saved.\nProceed?",
           .accept = "Open Project"}} {}

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

///
auto OpenFileDialog::GetTitle() const -> std::string {
  return "Open Project JSON";
}
}  // namespace vh::ponc::draw