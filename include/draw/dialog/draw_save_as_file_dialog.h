#ifndef VH_DRAW_SAVE_AS_FILE_DIALOG_H_
#define VH_DRAW_SAVE_AS_FILE_DIALOG_H_

#include "draw_i_file_dialog.h"

namespace esc::draw {
class SaveAsFileDialog : public IFileDialog {
 public:
  SaveAsFileDialog();

 private:
  void OnFileSelected(const AppState &app_state,
                      std::string file_path) const override;
};
}  // namespace esc::draw

#endif  // VH_DRAW_SAVE_AS_FILE_DIALOG_H_
