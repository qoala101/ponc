/**
 * PON Calculator https://github.com/qoala101/ponc
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_I_VIEW_H_
#define VH_PONC_DRAW_I_VIEW_H_

#include <string>

#include "cpp_non_copyable.h"
#include "cpp_scope_function.h"

namespace vh::ponc::draw {
///
class IView : public cpp::NonCopyable {
 public:
  ///
  virtual auto GetLabel() const -> std::string = 0;

  ///
  auto IsOpened() const -> bool;
  ///
  void Toggle();

 protected:
  ///
  auto DrawContentScope() -> cpp::ScopeFunction;

 private:
  ///
  bool opened_{};
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_I_VIEW_H_
