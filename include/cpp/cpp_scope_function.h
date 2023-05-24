/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_CPP_SCOPE_FUNCTION_H_
#define VH_PONC_CPP_SCOPE_FUNCTION_H_

#include <functional>

namespace vh::ponc::cpp {
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
}  // namespace vh::ponc::cpp

#endif  // VH_PONC_CPP_SCOPE_FUNCTION_H_
