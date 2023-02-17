#ifndef VH_ESC_APP_IMPL_H_
#define VH_ESC_APP_IMPL_H_

#include "esc_editor_context_handle.h"
#include "esc_state.h"

namespace esc {
class AppImpl {
 public:
  void OnFrame();

 private:
  std::shared_ptr<State> state_{};
  esc::EditorContextHandle editor_context_{};
};
}  // namespace esc

#endif  // VH_ESC_APP_IMPL_H_
