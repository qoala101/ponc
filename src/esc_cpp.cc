#include "esc_cpp.h"

#include <cassert>

namespace cpp {
void Expects(bool condition) { assert(condition); }

void Ensures(bool condition) { assert(condition); }

ScopeFunction::ScopeFunction(std::function<void()> end)
    : end_{(Expects(bool{end}), std::move(end))} {
  Ensures(bool{end_});
}

ScopeFunction::~ScopeFunction() {
  if (end_) {
    end_();
  }
}
}  // namespace cpp