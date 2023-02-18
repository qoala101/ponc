#include "coreui_i_pin_drawer.h"

namespace esc::coreui {
auto IPinDrawer::GetLabel() const -> std::string { return {}; }

auto IPinDrawer::GetFloat() -> float* { return nullptr; }

auto IPinDrawer::IsEditable() const -> bool { return false; }

auto IPinDrawer::GetPinId() const -> std::optional<ne::PinId> {
  return std::nullopt;
}
}  // namespace esc::coreui