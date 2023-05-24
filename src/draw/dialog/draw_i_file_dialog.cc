/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_i_file_dialog.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <string>
#include <string_view>

namespace vh::ponc::draw {
///
void IFileDialog::Open() {
  dialog_.SetTitle(GetTitle());
  dialog_.Open();
}

///
void IFileDialog::Draw(const Callbacks &callbacks) {
  dialog_.Display();
  FixWindowFlags();

  if (!dialog_.HasSelected()) {
    return;
  }

  callbacks.file_selected(dialog_.GetSelected().string());
  dialog_.ClearSelected();
}

///
IFileDialog::IFileDialog(const ImGui::FileBrowser &dialog) : dialog_{dialog} {}

///
// vh: Dialog has unique window name for each run, which pollutes settings INI
// file. This fixes it by manually setting NoSavedSettings flag which couldn't
// be set by other means.
void IFileDialog::FixWindowFlags() {
  if (dialog_window_ == nullptr) {
    auto *top_window = ImGui::GetTopMostPopupModal();

    if (top_window != nullptr) {
      const auto tow_window_name = std::string_view{top_window->Name};

      if (tow_window_name.starts_with(GetTitle() + "##filebrowser")) {
        dialog_window_ = top_window;
      }
    }
  }

  if (dialog_window_ != nullptr) {
    dialog_window_->Flags =
        // NOLINTNEXTLINE(*-signed-bitwise)
        dialog_window_->Flags | ImGuiWindowFlags_NoSavedSettings;
  }
}
}  // namespace vh::ponc::draw