#ifndef VH_CORE_ID_GENERATOR_H_
#define VH_CORE_ID_GENERATOR_H_

#include <imgui_node_editor.h>

namespace esc::core {
// ---
class IdGenerator {
 public:
  // ---
  explicit IdGenerator(uintptr_t next_value = 0);

  // ---
  template <typename T>
  auto GetNext() -> T {
    return T{next_value_++};
  }

 private:
  uintptr_t next_value_{};
};
}  // namespace esc::core

#endif  // VH_CORE_ID_GENERATOR_H_
