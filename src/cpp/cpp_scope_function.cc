/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "cpp_scope_function.h"

#include <utility>

namespace vh::ponc::cpp {
///
ScopeFunction::ScopeFunction(std::function<void()> destructor)
    : destructor_{std::move(destructor)} {}

///
ScopeFunction::~ScopeFunction() {
  if (const auto object_was_moved = !destructor_) {
    return;
  }

  destructor_();
}
}  // namespace vh::ponc::cpp