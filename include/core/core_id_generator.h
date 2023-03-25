#ifndef VH_PONC_CORE_ID_GENERATOR_H_
#define VH_PONC_CORE_ID_GENERATOR_H_

#include <imgui_node_editor.h>

#include <algorithm>
#include <vector>

#include "core_concepts.h"

namespace vh::ponc::core {
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
    std::generate(ids.begin(), ids.end(), [this]() { return Generate<T>(); });
    return ids;
  }

  ///
  auto Generate() { return next_id_++; }

  ///
  auto GetNextId() const { return next_id_; }

 private:
  ///
  uintptr_t next_id_{};
};
}  // namespace vh::ponc::core

#endif  // VH_PONC_CORE_ID_GENERATOR_H_
