#ifndef VH_DRAW_I_FILE_DIALOG_H_
#define VH_DRAW_I_FILE_DIALOG_H_

// clang-format off
#include <imgui.h>
// clang-format on

#include <imfilebrowser.h>

#include "draw_i_drawable.h"
#include "esc_app_state.h"

namespace esc::draw {
// ---
class IFileDialog : public IDrawable {
 public:
  // ---
  void Draw(AppState &app_state) override;

 protected:
  // ---
  explicit IFileDialog(const ImGui::FileBrowser &dialog);

 private:
  // ---
  virtual void OnFileSelected(AppState &app_state,
                              std::string file_path) const = 0;

  // ---
  void OnVisibleChanged() override;

  std::shared_ptr<AppState> app_state_{};
  ImGui::FileBrowser dialog_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_I_FILE_DIALOG_H_
