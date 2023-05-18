/**
 * PON Calculator https://github.com/qoala101/ponc
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_CPP_CALLBACKS_H_
#define VH_PONC_CPP_CALLBACKS_H_

#include <functional>

namespace vh::ponc::cpp {
///
template <typename T, typename... Args>
using Query = std::function<auto(Args...)->T>;

///
template <typename... Args>
using Signal = std::function<void(Args...)>;

///
template <typename Signature>
using Action = std::function<Signature>;
}  // namespace vh::ponc::cpp

#endif  // VH_PONC_CPP_CALLBACKS_H_
