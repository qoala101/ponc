/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_CORE_ID_GENERATOR_H_
#define VH_CORE_ID_GENERATOR_H_

#include <imgui_node_editor.h>

#include <vector>

namespace esc::core {
// ---
class IdGenerator {
 public:
  // ---
  explicit IdGenerator(uintptr_t next_value = 1);

  // ---
  template <typename Id>
  auto GetNext() {
    return Id{next_value_++};
  }

  // ---
  template <typename Id>
  auto GetNextN(int n) {
    auto ids = std::vector<Id>{};
    ids.reserve(n);

    for (auto i = 0; i < n; ++i) {
      ids.emplace_back(GetNext<Id>());
    }

    return ids;
  }

 private:
  // ---
  uintptr_t next_value_{};
};
}  // namespace esc::core

#endif  // VH_CORE_ID_GENERATOR_H_
