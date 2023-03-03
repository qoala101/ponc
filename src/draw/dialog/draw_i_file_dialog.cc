#include "draw_i_file_dialog.h"

namespace esc::draw {
///
void IFileDialog::Draw(const Hooks &hooks) {
  dialog_.Display();

  if (!dialog_.HasSelected()) {
    return;
  }

  hooks.file_selected(dialog_.GetSelected().string());
  dialog_.ClearSelected();
}

///
void IFileDialog::Show() { dialog_.Open(); }

///
IFileDialog::IFileDialog(const ImGui::FileBrowser &dialog) : dialog_{dialog} {}
}  // namespace esc::draw