/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_SAVE_AS_FILE_DIALOG_H_
#define VH_PONC_DRAW_SAVE_AS_FILE_DIALOG_H_

#include <string>

#include "draw_i_file_dialog.h"

namespace vh::ponc::draw {
///
class SaveAsFileDialog : public IFileDialog {
 public:
  ///
  SaveAsFileDialog();

 private:
  ///
  auto GetTitle() const -> std::string override;
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_SAVE_AS_FILE_DIALOG_H_
