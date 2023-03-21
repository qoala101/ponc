#ifndef VH_draw_question_dialog_H_
#define VH_draw_question_dialog_H_

#include <string>
#include <string_view>

#include "cpp_callbacks.h"

namespace esc::draw {
///
class QuestionDialog {
 public:
  ///
  struct Callbacks {
    ///
    cpp::Signal<> accepted{};
  };

  struct ConstructorArgs {
    std::string title{};
    std::string question{};
    std::string ok_label{};
  };

  explicit QuestionDialog(const ConstructorArgs &args);

  ///
  void Open();
  ///
  void Draw(const Callbacks &callbacks);

 private:
  ///
  bool open_requested_{};
  ///
  std::string title_{};
  std::string question_{};
  std::string ok_label_{};
};
}  // namespace esc::draw

#endif  // VH_draw_question_dialog_H_
