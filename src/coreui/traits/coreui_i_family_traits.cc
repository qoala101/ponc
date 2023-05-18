/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "coreui_i_family_traits.h"

namespace vh::ponc::coreui {
///
auto IFamilyTraits::GetGroupLabel() const -> std::string { return GetLabel(); }

///
auto IFamilyTraits::IsUserAccessible() -> bool { return true; }
}  // namespace vh::ponc::coreui