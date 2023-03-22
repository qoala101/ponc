#ifndef VH_PONC_DRAW_OPEN_FILE_DIALOG_H_
#define VH_PONC_DRAW_OPEN_FILE_DIALOG_H_

#include <filesystem>

#include "draw_i_file_dialog.h"
#include "draw_question_dialog.h"

namespace vh::ponc::draw {
///
class OpenFileDialog : public IFileDialog {
 public:
  ///
  OpenFileDialog();

  ///
  void Draw(const Callbacks &callbacks, bool requires_confirmation = false);

 private:
  ///
  QuestionDialog confirmation_dialog_;
  ///
  std::filesystem::path selected_file_path_{};
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_OPEN_FILE_DIALOG_H_
