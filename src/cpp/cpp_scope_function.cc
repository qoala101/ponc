#include "cpp_scope_function.h"

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