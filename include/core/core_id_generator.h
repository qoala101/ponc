#ifndef VH_CORE_ID_GENERATOR_H_
#define VH_CORE_ID_GENERATOR_H_

#include <bits/ranges_algo.h>
#include <imgui_node_editor.h>

#include <vector>

#include "core_concepts.h"

namespace esc::core {
///
class IdGenerator {
 public:
  ///
  explicit IdGenerator(uintptr_t next_id = 1);

  ///
  template <Id T>
  auto Generate() {
    return T{next_id_++};
  }

  ///
  template <Id T>
  auto GenerateN(int n) {
    auto ids = std::vector<T>(n);
    std::ranges::generate(ids, [this]() { return Generate<T>(); });
    return ids;
  }

  ///
  auto GetNextId() const { return next_id_; }

 private:
  ///
  uintptr_t next_id_{};
};
}  // namespace esc::core

#endif  // VH_CORE_ID_GENERATOR_H_
