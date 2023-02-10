#ifndef VH_DRAW_OPEN_FILE_DIALOG_H_
#define VH_DRAW_OPEN_FILE_DIALOG_H_

#include <memory>

#include "draw_i_file_dialog.h"
#include "esc_app_state.h"

namespace esc::draw {
class OpenFileDialog : public IFileDialog {
 public:
  explicit OpenFileDialog(std::shared_ptr<AppState> app_state);

 private:
  void OnFileSelected(std::string file_path) const override;
};
}  // namespace esc::draw

#endif  // VH_DRAW_OPEN_FILE_DIALOG_H_
