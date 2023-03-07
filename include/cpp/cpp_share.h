#ifndef VH_CPP_SHARE_H_
#define VH_CPP_SHARE_H_

#include <memory>
#include <utility>

namespace esc::cpp {
///
template <typename T>
auto Share(T t) {
  return std::make_shared<std::remove_cvref_t<T>>(std::move(t));
}
}  // namespace esc::cpp

#endif  // VH_CPP_SHARE_H_
