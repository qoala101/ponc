#include "draw_disable_if.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "cpp_scope_function.h"

namespace vh::ponc::draw {
///
auto DisableIf(bool condition) -> cpp::ScopeFunction {
  if (!condition) {
    return cpp::ScopeFunction{[]() {}};
  }

  ImGui::PushDisabled();
  return cpp::ScopeFunction{[]() { ImGui::PopDisabled(); }};
}

///
auto EnableIf(bool condition) -> cpp::ScopeFunction {
  return DisableIf(!condition);
}
}  // namespace vh::ponc::draw