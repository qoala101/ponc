#include "cpp_scope_function.h"

namespace esc::cpp {
ScopeFunction::ScopeFunction(std::function<void()> end)
    : destructor_{std::move(end)} {}

ScopeFunction::~ScopeFunction() {
  if (destructor_) {
    destructor_();
  }
}
}  // namespace esc::cpp