#include "draw_constants.h"

#include "imgui.h"

namespace esc::draw {
auto Constants::GetTextSymbolWidth() -> float {
  return ImGui::CalcTextSize("A").x;
}
}  // namespace esc::draw