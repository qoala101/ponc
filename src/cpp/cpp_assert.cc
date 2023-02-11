#include "cpp_assert.h"

#include <cassert>

namespace esc::cpp {
// NOLINTNEXTLINE(*-no-array-decay, *-array-to-pointer-decay)
void Expects(bool condition) { assert(condition); }

// NOLINTNEXTLINE(*-no-array-decay, *-array-to-pointer-decay)
void Ensures(bool condition) { assert(condition); }
}  // namespace esc::cpp