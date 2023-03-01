#ifndef VH_CPP_SAFE_POINTER_H_
#define VH_CPP_SAFE_POINTER_H_

#include <memory>

namespace esc::cpp {

template <typename T>
class SafePointer {
 public:
  auto operator*() const -> auto & {
    // NOLINTNEXTLINE(*-const-cast)
    return const_cast<SafePointer *>(this)->operator*();
  }

  auto operator*() -> auto & { return *t_; }

  auto operator->() const {
    // NOLINTNEXTLINE(*-const-cast)
    return const_cast<SafePointer *>(this)->operator->();
  }

  auto operator->() { return t_; }

 private:
  template <typename U>
  friend class SafePointerFactory;

  SafePointer(T *t, const std::shared_ptr<void *> &shared)
      : t_{t}, weak_instance_{shared} {}

  T *t_{};

  std::weak_ptr<void *> weak_instance_{};
};

template <typename T>
class EnableSafePointer {
 public:
  auto CreateSafePointer() const -> SafePointer<T> {
    return SafePointer{t_, instance_};
  }

 protected:
  explicit EnableSafePointer(T *t) : t_{t} {}

 private:
  T *t_{};

  std::shared_ptr<void *> instance_{};
};
}  // namespace esc::cpp

#endif  // VH_CPP_SAFE_POINTER_H_
