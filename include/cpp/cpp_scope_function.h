#ifndef VH_CPP_SCOPE_FUNCTION_H_
#define VH_CPP_SCOPE_FUNCTION_H_

#include <functional>

namespace esc::cpp {
///
class ScopeFunction {
 public:
  ///
  explicit ScopeFunction(std::function<void()> destructor = []() {});

  ///
  ScopeFunction(const ScopeFunction&) = delete;
  ///
  ScopeFunction(ScopeFunction&&) noexcept = default;

  ///
  auto operator=(const ScopeFunction&) -> ScopeFunction& = delete;
  ///
  auto operator=(ScopeFunction&&) noexcept -> ScopeFunction& = default;

  ///
  ~ScopeFunction();

 private:
  ///
  std::function<void()> destructor_{};
};
}  // namespace esc::cpp

#endif  // VH_CPP_SCOPE_FUNCTION_H_
