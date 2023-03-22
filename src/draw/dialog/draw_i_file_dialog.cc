#include "draw_i_file_dialog.h"

namespace vh::ponc::draw {
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
}  // namespace vh::ponc::draw