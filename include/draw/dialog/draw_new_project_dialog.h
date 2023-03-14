#ifndef VH_DRAW_NEW_PROJECT_DIALOG_H_
#define VH_DRAW_NEW_PROJECT_DIALOG_H_

#include <string_view>

#include "cpp_callbacks.h"

namespace esc::draw {
///
class NewProjectDialog {
 public:
  ///
  struct Callbacks {
    ///
    cpp::Signal<> accepted{};
  };

  ///
  void Open();
  ///
  void Draw(const Callbacks &callbacks);

 private:
  ///
  bool open_requested_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_NEW_PROJECT_DIALOG_H_
