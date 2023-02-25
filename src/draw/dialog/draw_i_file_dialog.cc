#include "draw_i_file_dialog.h"

namespace esc::draw {
void IFileDialog::Show() { dialog_.Open(); }

void IFileDialog::Draw(const SignalFileSelected &signal_file_selected) {
  dialog_.Display();

  if (!dialog_.HasSelected()) {
    return;
  }

  signal_file_selected(dialog_.GetSelected().string());
  dialog_.ClearSelected();
}

IFileDialog::IFileDialog(const ImGui::FileBrowser &dialog) : dialog_{dialog} {}
}  // namespace esc::draw