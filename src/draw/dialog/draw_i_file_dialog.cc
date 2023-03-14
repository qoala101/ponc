#include "draw_i_file_dialog.h"

namespace esc::draw {
///
void IFileDialog::Open() { dialog_.Open(); }

///
void IFileDialog::Draw(const Callbacks &callbacks) {
  dialog_.Display();

  if (!dialog_.HasSelected()) {
    return;
  }

  callbacks.file_selected(dialog_.GetSelected().string());
  dialog_.ClearSelected();
}

///
IFileDialog::IFileDialog(const ImGui::FileBrowser &dialog) : dialog_{dialog} {}
}  // namespace esc::draw