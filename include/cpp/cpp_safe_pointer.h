#ifndef VH_CPP_SAFE_POINTER_H_
#define VH_CPP_SAFE_POINTER_H_

#include <memory>

#include "cpp_assert.h"

namespace esc::cpp {
///
template <typename T>
class SafePointer {
 public:
  ///
  auto operator*() const -> auto & {
    // NOLINTNEXTLINE(*-const-cast)
    return const_cast<SafePointer *>(this)->operator*();
  }

  ///
  auto operator*() -> auto & {
    Expects(!factory_.expired());
    return *t_;
  }

  ///
  auto operator->() const {
    // NOLINTNEXTLINE(*-const-cast)
    return const_cast<SafePointer *>(this)->operator->();
  }

  ///
  auto operator->() {
    Expects(!factory_.expired());
    return t_;
  }

 private:
  ///
  friend class SafePointerFactory;

  ///
  SafePointer(T *t, const std::shared_ptr<void *> &shared)
      : t_{t}, factory_{shared} {}

  ///
  T *t_{};
  ///
  std::weak_ptr<void *> factory_{};
};

///
class SafePointerFactory {
 public:
  ///
  template <typename T>
  auto CreateSafePointer(T *t) const -> SafePointer<T> {
    return SafePointer{t, instance_};
  }

 private:
  ///
  std::shared_ptr<void *> instance_{};
};
}  // namespace esc::cpp

#endif  // VH_CPP_SAFE_POINTER_H_
