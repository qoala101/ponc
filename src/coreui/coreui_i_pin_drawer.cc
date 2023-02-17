#include "coreui_i_pin_drawer.h"

namespace esc::coreui {
auto IPinDrawer::GetLabel() const -> std::string { return {}; }

auto IPinDrawer::GetFloat() -> float* { return nullptr; }

auto IPinDrawer::IsEditable() const -> bool { return false; }
}  // namespace esc::draw