#ifndef VH_ESC_CPP_H_
#define VH_ESC_CPP_H_

#include <functional>
#include <utility>

namespace cpp {
void Expects(bool condition);
void Ensures(bool condition);

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

class ScopeFunction {
 public:
  explicit ScopeFunction(std::function<void()> end);

  ScopeFunction(const ScopeFunction&) = delete;
  ScopeFunction(ScopeFunction&&) noexcept = default;

  auto operator=(const ScopeFunction&) noexcept -> ScopeFunction& = delete;
  auto operator=(ScopeFunction&&) noexcept -> ScopeFunction& = default;

  ~ScopeFunction();

 private:
  std::function<void()> end_{};
};
}  // namespace cpp

#endif  // VH_ESC_CPP_H_
