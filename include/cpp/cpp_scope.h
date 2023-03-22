#ifndef VH_PONC_CPP_SCOPE_H_
#define VH_PONC_CPP_SCOPE_H_

#include <concepts>
#include <utility>

namespace vh::ponc::cpp {
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
}  // namespace vh::ponc::cpp

#endif  // VH_PONC_CPP_SCOPE_H_
