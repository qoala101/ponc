#ifndef VH_CPP_HOOK_H_
#define VH_CPP_HOOK_H_

#include <functional>

namespace esc::cpp {
///
template <typename Signature>
using Hook = std::function<Signature>;
}  // namespace esc::cpp

#endif  // VH_CPP_HOOK_H_
