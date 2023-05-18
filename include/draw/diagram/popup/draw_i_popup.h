/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_I_POPUP_H_
#define VH_PONC_DRAW_I_POPUP_H_

#include <imgui.h>

#include <optional>
#include <string>

#include "cpp_callbacks.h"
#include "cpp_non_copyable.h"
#include "cpp_scope_function.h"

namespace vh::ponc::draw {
///
class IPopup : public cpp::NonCopyable {
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
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_I_POPUP_H_
