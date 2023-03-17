#ifndef VH_DRAW_I_VIEW_H_
#define VH_DRAW_I_VIEW_H_

#include <string>

#include "cpp_interface.h"
#include "cpp_scope_function.h"

namespace esc::draw {
///
class IView : public cpp::Interface {
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
}  // namespace esc::draw

#endif  // VH_DRAW_I_VIEW_H_
