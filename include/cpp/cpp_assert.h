#ifndef VH_CPP_ASSERT_H_
#define VH_CPP_ASSERT_H_

#include <cassert>

// ---
// NOLINTNEXTLINE(*-no-array-decay, *-array-to-pointer-decay)
#define Expects(condition) (assert(condition))

// ---
// NOLINTNEXTLINE(*-no-array-decay, *-array-to-pointer-decay)
#define Ensures(condition) (assert(condition))

#endif  // VH_CPP_ASSERT_H_
