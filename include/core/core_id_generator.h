#ifndef VH_CORE_ID_GENERATOR_H_
#define VH_CORE_ID_GENERATOR_H_

#include <imgui_node_editor.h>

#include <vector>

namespace esc::core {
class IdGenerator {
 public:
  explicit IdGenerator(uintptr_t next_id = 1);

  template <typename Id>
  auto Generate() {
    return Id{next_id_++};
  }

  template <typename Id>
  auto GenerateN(int n) {
    auto ids = std::vector<Id>{};
    ids.reserve(n);

    for (auto i = 0; i < n; ++i) {
      ids.emplace_back(Generate<Id>());
    }

    return ids;
  }

  auto GetNextId() const { return next_id_; }

 private:
  uintptr_t next_id_{};
};
}  // namespace esc::core

#endif  // VH_CORE_ID_GENERATOR_H_
