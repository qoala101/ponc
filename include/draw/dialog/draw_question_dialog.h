/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_QUESTION_DIALOG_H_
#define VH_PONC_DRAW_QUESTION_DIALOG_H_

#include <string>

#include "cpp_callbacks.h"

namespace vh::ponc::draw {
///
class QuestionDialog {
 public:
  ///
  struct Callbacks {
    ///
    cpp::Signal<> accepted{};
  };

  ///
  struct Labels {
    ///
    std::string title{};
    ///
    std::string question{};
    ///
    std::string accept{};
  };

  explicit QuestionDialog(Labels labels);

  ///
  void Open();
  ///
  void Draw(const Callbacks &callbacks);

 private:
  ///
  bool open_requested_{};
  ///
  Labels labels_{};
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_QUESTION_DIALOG_H_
