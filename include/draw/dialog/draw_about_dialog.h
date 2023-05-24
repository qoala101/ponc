/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_ABOUT_DIALOG_H_
#define VH_PONC_DRAW_ABOUT_DIALOG_H_

namespace vh::ponc::draw {
///
class AboutDialog {
 public:
  ///
  void Open();
  ///
  void Draw();

 private:
  ///
  bool open_requested_{};
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_ABOUT_DIALOG_H_
