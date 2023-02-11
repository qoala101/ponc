#include "draw_i_file_dialog.h"

namespace esc::draw {
void IFileDialog::Draw(State &state) {
  dialog_.Display();

  if (!dialog_.HasSelected()) {
    return;
  }

  OnFileSelected(state, dialog_.GetSelected().string());
  dialog_.ClearSelected();
}

IFileDialog::IFileDialog(const ImGui::FileBrowser &dialog) : dialog_{dialog} {}

void IFileDialog::OnVisibleChanged() {
  if (IsVisible()) {
    dialog_.Open();
  }
}
}  // namespace esc::draw