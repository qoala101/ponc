/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_DISABLE_IF_H_
#define VH_PONC_DRAW_DISABLE_IF_H_

#include "cpp_scope_function.h"

namespace vh::ponc::draw {
///
auto DisableIf(bool condition) -> cpp::ScopeFunction;
///
auto EnableIf(bool condition) -> cpp::ScopeFunction;
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_DISABLE_IF_H_
