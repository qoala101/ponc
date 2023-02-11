#ifndef VH_CPP_SCOPE_H_
#define VH_CPP_SCOPE_H_

#include <utility>

namespace esc::cpp {
template <typename End, typename Start = decltype([]() {})>
class Scope {
 public:
  explicit Scope(End end) : end_{std::move(end)} {}

  Scope(const Start& start, End end) : Scope{std::move(end)} { start(); }

  Scope(const Scope&) = delete;
  Scope(Scope&&) noexcept = delete;

  auto operator=(const Scope&) noexcept -> Scope& = delete;
  auto operator=(Scope&&) noexcept -> Scope& = delete;

  ~Scope() { end_(); }

 private:
  End end_;
};
}  // namespace esc::cpp

#endif  // VH_CPP_SCOPE_H_
