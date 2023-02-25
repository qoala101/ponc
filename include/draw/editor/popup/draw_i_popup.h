#ifndef VH_DRAW_I_POPUP_H_
#define VH_DRAW_I_POPUP_H_

#include <string>

#include "cpp_interface.h"
#include "cpp_scope_function.h"

namespace esc::draw {
class IPopup : public cpp::Interface {
 public:
  void Show();

 protected:
  auto DrawContentScope() const -> std::pair<bool, cpp::ScopeFunction>;

 private:
  virtual auto GetLabel() const -> std::string = 0;
};
}  // namespace esc::draw

#endif  // VH_DRAW_I_POPUP_H_
