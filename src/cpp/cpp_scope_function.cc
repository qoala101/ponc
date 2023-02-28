#include "cpp_scope_function.h"

namespace esc::cpp {
///
ScopeFunction::ScopeFunction(std::function<void()> destructor)
    : destructor_{std::move(destructor)} {}

///
ScopeFunction::~ScopeFunction() { destructor_(); }
}  // namespace esc::cpp