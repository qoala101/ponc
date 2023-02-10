#ifndef VH_ESC_ID_GENERATOR_H_
#define VH_ESC_ID_GENERATOR_H_

#include <imgui_node_editor.h>

namespace esc {
class IdGenerator {
 public:
  template <typename T>
  auto GetNext  () -> T {
    return T{next_value_++};
  }

 private:
  uintptr_t next_value_{};
};
}  // namespace esc

#endif  // VH_ESC_ID_GENERATOR_H_
