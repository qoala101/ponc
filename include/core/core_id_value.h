#ifndef VH_PONC_CORE_ID_VALUE_H_
#define VH_PONC_CORE_ID_VALUE_H_

#include "core_concepts.h"

namespace vh::ponc::core {
///
template <Id T>
using IdValue = decltype(T{}.Get());
///
using UnspecifiedIdValue = uintptr_t;
}  // namespace vh::ponc::core

#endif  // VH_PONC_CORE_ID_VALUE_H_
