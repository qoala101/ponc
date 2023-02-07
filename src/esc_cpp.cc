#include "esc_cpp.h"

#include <cassert>

namespace cpp {
void Expects(bool condition) { assert(condition); }

void Ensures(bool condition) { assert(condition); }
}  // namespace cpp