/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_OPEN_FILE_DIALOG_H_
#define VH_PONC_DRAW_OPEN_FILE_DIALOG_H_

#include <filesystem>
#include <string>

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
  auto GetTitle() const -> std::string override;

  ///
  QuestionDialog confirmation_dialog_;
  ///
  std::filesystem::path selected_file_path_{};
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_OPEN_FILE_DIALOG_H_
