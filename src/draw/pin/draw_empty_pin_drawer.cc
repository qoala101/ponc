#include "draw_empty_pin_drawer.h"

namespace esc::draw {
auto EmptyPinDrawer::GetLabel() const -> std::string { return {}; }

auto EmptyPinDrawer::GetFloat() const -> float* { return nullptr; }

auto EmptyPinDrawer::IsEditable() const -> bool { return false; }
}  // namespace esc::draw