#ifndef VH_DRAW_I_POPUP_H_
#define VH_DRAW_I_POPUP_H_

#include <optional>
#include <string>

#include "cpp_callbacks.h"
#include "cpp_interface.h"
#include "cpp_scope_function.h"
#include "imgui.h"

namespace esc::draw {
///
class IPopup : public cpp::Interface {
 public:
  ///
  struct Callbacks {
    ///
    std::optional<cpp::Signal<>> closed{};
  };

  //
  auto IsOpened() const -> bool;
  ///
  void Open();

 protected:
  ///
  IPopup();

  ///
  auto DrawContentScope(std::string_view title = {},
                        const Callbacks &callbacks = {{}})
      -> cpp::ScopeFunction;

 private:
  ///
  ImGuiID id_{};
  ///
  bool opened_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_I_POPUP_H_
