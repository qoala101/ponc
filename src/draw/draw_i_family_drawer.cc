#include "draw_i_family_drawer.h"

namespace esc::draw {
auto IFamilyDrawer::GetGroupLabel() const -> std::string { return GetLabel(); }

auto IFamilyDrawer::IsUserAccessible() -> bool { return true; }
}  // namespace esc::draw