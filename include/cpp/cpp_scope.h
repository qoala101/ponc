#ifndef VH_CPP_SCOPE_H_
#define VH_CPP_SCOPE_H_

#include <concepts>
#include <utility>

namespace esc::cpp {
///
template <std::invocable Destructor>
class Scope {
 public:
  ///
  explicit Scope(Destructor destructor) : destructor_{std::move(destructor)} {}

  ///
  Scope(const Scope&) = delete;
  ///
  Scope(Scope&&) noexcept = delete;

  ///
  auto operator=(const Scope&) -> Scope& = delete;
  ///
  auto operator=(Scope&&) noexcept -> Scope& = delete;

  ///
  ~Scope() { destructor_(); }

 private:
  ///
  Destructor destructor_;
};
}  // namespace esc::cpp

#endif  // VH_CPP_SCOPE_H_
