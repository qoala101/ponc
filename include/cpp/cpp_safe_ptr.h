#ifndef VH_PONC_CPP_SAFE_PTR_H_
#define VH_PONC_CPP_SAFE_PTR_H_

#include <memory>
#include <type_traits>

#include "cpp_assert.h"

namespace vh::ponc::cpp {
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
  SafeOwner() : instance_{std::make_shared<void *>()} {}

  ///
  SafeOwner(const SafeOwner &) = delete;
  ///
  SafeOwner(SafeOwner &&) noexcept = delete;

  ///
  auto operator=(const SafeOwner &) -> SafeOwner & = delete;
  ///
  auto operator=(SafeOwner &&) noexcept -> SafeOwner & = delete;

  ///
  ~SafeOwner() = default;

  ///
  auto MakeSafe(auto *t) const { return SafePtr{t, instance_}; }

 private:
  ///
  std::shared_ptr<void *> instance_{};
};
}  // namespace vh::ponc::cpp

#endif  // VH_PONC_CPP_SAFE_PTR_H_
