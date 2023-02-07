#ifndef VH_ESC_CPP_H_
#define VH_ESC_CPP_H_

#include <utility>

namespace cpp {
void Expects(bool condition);
void Ensures(bool condition);

template <typename Start, typename End>
class Scope {
 public:
  Scope(const Start& start, End end) : end_{std::move(end)} { start(); }

  Scope(const Scope&) = delete;
  Scope(Scope&&) noexcept = delete;

  auto operator=(const Scope&) noexcept -> Scope& = delete;
  auto operator=(Scope&&) noexcept -> Scope& = delete;

  ~Scope() { end_(); }

 private:
  End end_;
};
}  // namespace cpp

#endif  // VH_ESC_CPP_H_
