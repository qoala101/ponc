#include "esc_auto_incrementable.h"

namespace esc {
AutoIncrementable::AutoIncrementable(int initial_value)
    : value_{initial_value} {}

auto AutoIncrementable::GetNext() -> int { return value_++; }
}  // namespace esc