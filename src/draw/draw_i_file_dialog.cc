#include "draw_i_file_dialog.h"

namespace esc::draw {
IFileDialog::IFileDialog(const ImGui::FileBrowser &dialog) : dialog_{dialog} {}

void IFileDialog::Draw() {
  dialog_.Display();

  if (!dialog_.HasSelected()) {
    return;
  }

  OnFileSelected(dialog_.GetSelected().string());
  dialog_.ClearSelected();
}

void IFileDialog::OnVisibleChanged() {
  if (IsVisible()) {
    dialog_.Open();
  }
}
}  // namespace esc::draw