#ifndef VH_CPP_SAFE_PTR_H_
#define VH_CPP_SAFE_PTR_H_

#include <memory>
#include <type_traits>

#include "cpp_assert.h"

namespace esc::cpp {
///
template <typename T>
class SafePtr {
 public:
  ///
  auto operator*() const -> auto & {
    // NOLINTNEXTLINE(*-const-cast)
    return const_cast<SafePtr *>(this)->operator*();
  }

  ///
  auto operator*() -> auto & {
    Expects(!owner_.expired());
    return *t_;
  }

  ///
  auto operator->() const {
    // NOLINTNEXTLINE(*-const-cast)
    return const_cast<SafePtr *>(this)->operator->();
  }

  ///
  auto operator->() {
    Expects(!owner_.expired());
    return t_;
  }

 private:
  ///
  friend class SafeOwner;

  ///
  SafePtr(T *t, const std::shared_ptr<void *> &owner) : t_{t}, owner_{owner} {}

  ///
  T *t_{};
  ///
  std::weak_ptr<void *> owner_{};
};

///
class SafeOwner {
 public:
  ///
  auto MakeSafe(auto *t) const { return SafePtr{t, instance_}; }

 private:
  ///
  std::shared_ptr<void *> instance_{};
};
}  // namespace esc::cpp

#endif  // VH_CPP_SAFE_PTR_H_
