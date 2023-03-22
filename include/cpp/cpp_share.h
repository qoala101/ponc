#ifndef VH_PONC_CPP_SHARE_H_
#define VH_PONC_CPP_SHARE_H_

#include <memory>
#include <utility>

namespace vh::ponc::cpp {
///
template <typename T>
auto Share(T t) {
  return std::make_shared<std::remove_cvref_t<T>>(std::move(t));
}
}  // namespace vh::ponc::cpp

#endif  // VH_PONC_CPP_SHARE_H_
