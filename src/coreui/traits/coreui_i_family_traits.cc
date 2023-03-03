#include "coreui_i_family_traits.h"

namespace esc::coreui {
///
auto IFamilyTraits::GetGroupLabel() const -> std::string { return GetLabel(); }

///
auto IFamilyTraits::IsUserAccessible() -> bool { return true; }
}  // namespace esc::coreui