#ifndef VH_DRAW_I_POPUP_H_
#define VH_DRAW_I_POPUP_H_

#include <optional>
#include <string>

#include "cpp_callbacks.h"
#include "cpp_interface.h"
#include "cpp_scope_function.h"

namespace esc::draw {
///
class IPopup : public cpp::Interface {
 public:
  ///
  struct Callbacks {
    ///
    std::optional<cpp::Signal<>> closed{};
  };

  ///
  void Open();
  //
  auto IsOpened() const -> bool;

 protected:
  ///
  IPopup();

  ///
  auto DrawContentScope(const Callbacks &callbacks = {{}})
      -> cpp::ScopeFunction;

 private:
  ///
  virtual auto GetLabel() const -> std::string = 0;

  ///
  uintptr_t id_{};
  ///
  bool opened_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_I_POPUP_H_
