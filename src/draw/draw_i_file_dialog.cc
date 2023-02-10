#include "draw_i_file_dialog.h"

namespace esc::draw {
void IFileDialog::Draw() {
  dialog_.Display();

  if (!dialog_.HasSelected()) {
    return;
  }

  OnFileSelected(dialog_.GetSelected().string());
  dialog_.ClearSelected();
}

IFileDialog::IFileDialog(std::shared_ptr<AppState> app_state,
                         const ImGui::FileBrowser &dialog)
    : app_state_{std::move(app_state)}, dialog_{dialog} {}

auto IFileDialog::GetAppState() const -> AppState & { return *app_state_; }

void IFileDialog::OnVisibleChanged() {
  if (IsVisible()) {
    dialog_.Open();
  }
}
}  // namespace esc::draw