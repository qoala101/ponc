#ifndef VH_DRAW_OPEN_FILE_DIALOG_H_
#define VH_DRAW_OPEN_FILE_DIALOG_H_

#include "draw_i_file_dialog.h"

namespace esc::draw {
class OpenFileDialog : public IFileDialog {
 public:
  OpenFileDialog();

 private:
  void OnFileSelected(coreui::Frame &frame,
                      std::string file_path) const override;
};
}  // namespace esc::draw

#endif  // VH_DRAW_OPEN_FILE_DIALOG_H_
