#ifndef VH_CPP_CALLBACKS_H_
#define VH_CPP_CALLBACKS_H_

#include <functional>

namespace esc::cpp {
///
template <typename T, typename... Args>
using Query = std::function<auto(Args...)->T>;

///
template <typename... Args>
using Signal = std::function<void(Args...)>;

///
template <typename Signature>
using Action = std::function<Signature>;
}  // namespace esc::cpp

#endif  // VH_CPP_CALLBACKS_H_
