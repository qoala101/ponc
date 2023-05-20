/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_CORE_ID_GENERATOR_H_
#define VH_PONC_CORE_ID_GENERATOR_H_

#include <algorithm>
#include <vector>

#include "core_concepts.h"
#include "core_id_value.h"

namespace vh::ponc::core {
///
class IdGenerator {
 public:
  ///
  explicit IdGenerator(UnspecifiedIdValue next_id = 1);

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
  UnspecifiedIdValue next_id_{};
};
}  // namespace vh::ponc::core

#endif  // VH_PONC_CORE_ID_GENERATOR_H_
