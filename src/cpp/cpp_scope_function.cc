#include "cpp_scope_function.h"

namespace esc::cpp {
ScopeFunction::ScopeFunction(std::function<void()> end)
    : end_{std::move(end)} {}

ScopeFunction::~ScopeFunction() {
  if (end_) {
    end_();
  }
}
}  // namespace esc::cpp