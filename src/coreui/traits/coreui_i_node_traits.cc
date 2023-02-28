#include "coreui_i_node_traits.h"

namespace esc::coreui {
///
auto INodeTraits::HasHeader() const -> bool { return true; }

///
auto INodeTraits::CreatePinTraits() const
    -> std::vector<std::unique_ptr<IPinTraits>> {
  return {};
}
}  // namespace esc::coreui